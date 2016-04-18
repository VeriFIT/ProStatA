/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * All global variables are implicitly initialized to 0 or nullptr.
 * Pass modifies static local variable.
 * Aggregate initializers are split with GEP as ConstantExpr
 * 
 * opt -S -load ./passes_build/libglobalvars.so -global-vars source.ll -o source2.ll

without optimalization: llvm insert allways in @main (an implicit-return-zero function)
  %1 = alloca i32, align 4
  store i32 0, i32* %1
for possible implicit return 0

 */

#include "global_vars.hh"
#include "passes.hh"

#include <cstdint> // __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IntrinsicInst.h"

using namespace llvm;

#define DEBUG_TYPE "glob-vars"

STATISTIC(NumCopyGV, "Number of non-constant global variables");
STATISTIC(NumInitGV, "Number of initialized global variables");

// interface
ModulePass *createGlobalVarsPass() {
  return new GlobalVarsPass();
}

bool GlobalVarsPass::doInitialization(Module &) {
#ifdef DEBUG
	errs() << "GLOBAL-VARS-START\n";
#endif
	return true;
}

void GlobalVarsPass::printLinkage(enum GlobalValue::LinkageTypes l) {
	switch (l) {
		case GlobalValue::LinkageTypes::ExternalLinkage: errs() << "extern"; break;
		case GlobalValue::LinkageTypes::InternalLinkage: errs() << "intern"; break;
		case GlobalValue::LinkageTypes::PrivateLinkage : errs() << "privat"; break;
		case GlobalValue::LinkageTypes::CommonLinkage  : errs() << "common"; break;
		default: errs() << "UNKOWN"; break;
	}
}

void GlobalVarsPass::printGlobalVar(GlobalVariable *GV) {
	errs() << "<GV<<<<"
	       << "\t<type:" << GV->getValueID()
	       << "\t<name:" << GV->getName()
	       << "\tdeclar:" << ( (GV->isDeclaration())? "Y" : "N" )
	       << "\t<link:" << ( (GV->hasExternalLinkage())? "Y" : "N" ); 
	printLinkage(GV->getLinkage());
	errs() << "\t<const:" << ( (GV->isConstant())? "Y" : "N" )
	       << "\t<init:" << ( (GV->hasInitializer())? "Y" : "N" ) << "+uniq" << ( (GV->hasUniqueInitializer())? "Y" : "N" ) << "+def" << ( (GV->hasDefinitiveInitializer())? "Y" : "N" ) << "+exter" << ( (GV->isExternallyInitialized())? "Y" : "N" ) 
	       << "\t<visible" << ( (GV->hasDefaultVisibility())? "D" : ( (GV->hasHiddenVisibility())? "H" : ( (GV->hasProtectedVisibility())? "P" : "U") ) )
//	       << "\t<section" << GV->getSection()
	       << "\n";
}

void GlobalVarsPass::splitAggregateConstant(IRBuilder<> *builder, Constant *c, Value *ptr) {

	Type *cTy = c->getType();

	if (!cTy->isAggregateType()) { //simple constant
		builder->CreateStore(c, ptr, false);
		return;
	}

	// aggregate type
	unsigned numElms;
	if (isa<StructType>(cTy))
		numElms = cTy->getStructNumElements();
	else if (isa<ArrayType>(cTy))
		numElms = cTy->getArrayNumElements();
	else {
		errs() << getPassName() << ": Error: Unsupported type for aggregate constant \'" 
		       << *c << "\'\n";
		builder->CreateStore(c, ptr, false); // insert not change
		return;
	}

	for (unsigned i = 0; i < numElms; ++i) {
	
		Constant *elmVal = c->getAggregateElement(i);
		Type *elmTy = elmVal->getType();
		Value *elmPtr = builder->CreateConstGEP2_32(cTy, ptr, /*idx0*/0, /*idx1*/i, "");
		//builder->CreateStructGEP(cTy, ptr,/*idx*/i, "");

		if (elmTy->isAggregateType()) { // recursion
			splitAggregateConstant(builder, elmVal, elmPtr);
		} else {
			builder->CreateStore(/*value*/ elmVal,/*ptr*/ elmPtr, /*isVolatile*/false);
		}
	}
}


// one source file as one Module
// return true, if the module was modified (there are global variables)
// etc
bool GlobalVarsPass::runOnModule(Module &M) {
#ifdef DEBUG
	errs() << "GLOBAL-VARS-MODIFICATION\n";
#endif
	bool changeEC = false;
	
	Function *mainF;

	if ((mainF = M.getFunction("main")) == nullptr || M.getGlobalList().size() == 0) 
	{ // don't exist main function or global variables
		return changeEC;
	}
	
	changeEC = true;
	
	// create new function
	Constant *c = M.getOrInsertFunction("__initGlobalVar", 
		/* ret type */ Type::getVoidTy(M.getContext()),
		/* argument */ //Type::getVoidTy(M.getContext()),
		               nullptr);
	Function *initGVF = cast<Function>(c);

	// insert call function to main (as first instruction)
	CallInst::Create(/*called*/initGVF, /*ret*/"", /*before*/mainF->begin()->getFirstNonPHI());

	// beginning of function
	BasicBlock* blockGVF = BasicBlock::Create(getGlobalContext(), "entry", initGVF);
	IRBuilder<> builder(blockGVF);
	
	// insert store instructions and zero-initializer
	for (Module::global_iterator GV = M.global_begin(), GVE = M.global_end(); GV != GVE; ++GV)
	{
#ifdef DEBUG
		//errs() << "<GV<<<<<ID:" << M.getMDKindID(GV->getName()) ;
		printGlobalVar(&*GV);
#endif

		// copy initializer
		if (GV->isConstant()==false && GV->isDeclaration()==false) {
			//builder.CreateStore(/*value*/ GV->getInitializer() ,/*ptr*/ &*GV, /*isVolatile*/false);
			splitAggregateConstant(&builder, GV->getInitializer(), &*GV);
			++NumCopyGV;
		}

		// nuluj inicializator
		if (GV->isConstant()==false && GV->isDeclaration()==false && GV->hasUniqueInitializer()==true) {
			Constant *init = GV->getInitializer();
			if (!init->isNullValue()) {
				GV->setInitializer(Constant::getNullValue(GV->getValueType()));
				++NumInitGV;
			}
		}
	}

	// end of inserted function
	builder.CreateRetVoid();
	
	return changeEC;
}

bool GlobalVarsPass::doFinalization (Module &) {
#ifdef DEBUG
	errs() << "GLOBAL-VARS-END\n";
#endif
	return true;
}

// opt pass registration
char GlobalVarsPass::ID = 0;
static RegisterPass<GlobalVarsPass> X("global-vars", "Global Variable Initialization");

#undef  DEBUG_TYPE
