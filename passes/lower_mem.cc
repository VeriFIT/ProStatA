/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate llvm.memset / llvm.memcpy / (llvm.memmove) if possible
 * create corresponding function
 * !!! don't copy padding
 * 
 * TODO: copy dbg info
 */

#include "lower_mem.hh"
#include "passes.hh"

#include <cstdint> // __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallPtrSet.h"

using namespace llvm;

#define DEBUG_TYPE "lowermem"

STATISTIC(NumElimGV, "Number of eliminated \"global\" variables");
STATISTIC(NumElimMSet, "Number of eliminated memset");
STATISTIC(NumElimMCpy, "Number of eliminated memcpy");
//STATISTIC(NumElimMMove, "Number of eliminated memmove");

// interface
ModulePass *createLowerMemIntrinsicPass(bool justGV) {

	return new LowerMemIntrinsicPass(justGV);
}

bool LowerMemIntrinsicPass::doInitialization(Module &M) {

#ifdef DEBUG
	errs() << "LOWER-MEM-START\n";
#endif
	DL = new DataLayout(&M);
	return true;
}

// not eliminate mem if (return false): 
//    volatile
//    len type != mem len
//  [ dest align != mem align (or unknown) ]
bool LowerMemIntrinsicPass::testMem(MemIntrinsic *in) {

	if (in->isVolatile()) // behavior ?
		return false;

	auto dest = in->getDest();

	uint64_t typeLen = DL->getTypeSizeInBits(dest->getType()->getPointerElementType())/8;
	if (ConstantInt *len = dyn_cast<ConstantInt>(in->getLength())) {
		if (len->getValue().getZExtValue() != typeLen) {
			return false;
		}
	} else { // not constant
		return false;
	}

	unsigned align = in->getAlignment();
	if (align != 0 && align != 1) { // 0/1 => no alignment
		if (GlobalVariable *gv = dyn_cast<GlobalVariable>(dest)) {
			if (gv->getAlignment() != align)
				return false;
		} else if (AllocaInst *a = dyn_cast<AllocaInst>(dest)) {
			if (a->getAlignment() != align)
				return false;
		} // or unknown
	}

	return true;
}

// not eliminate mem if (return false): 
//    volatile
//    src type != dest type
//    len type != mem len
//  [ src align != dest align != mem align (or unknown) ]
bool LowerMemIntrinsicPass::testMemTransfer(MemTransferInst *in) {

	auto dest = in->getDest();
	auto src = in->getSource();

	if (dest->getType()->getPointerElementType() != src->getType()->getPointerElementType())
		return false;

	unsigned align = in->getAlignment();
	if (align != 0 && align != 1) { // 0/1 => no alignment
		if (GlobalVariable *gv = dyn_cast<GlobalVariable>(src)) {
			if (gv->getAlignment() != align)
				return false;
		} else if (AllocaInst *a = dyn_cast<AllocaInst>(src)) {
			if (a->getAlignment() != align)
				return false;
		} // or unknown
	}

	return testMem(in);
}

// not eliminate mem if (return false): 
//    volatile
//    not zero
//    len type != mem len
//  [ dest align != mem align (or unknown) ]
bool LowerMemIntrinsicPass::testMemSet(MemSetInst *in) {

	if (ConstantInt *val = dyn_cast<ConstantInt>(in->getValue())) {
		if(!val->isNullValue())
			return false;
	}
	return testMem(in);
}

// replace memcpy/memove -> load + store (if replace -> true)
// TODO: memove
bool LowerMemIntrinsicPass::replaceMemTransfer(MemTransferInst *in) {

	auto dest = in->getDest();
	auto src = in->getSource();
	auto rawdest = in->getRawDest();
	auto rawsrc = in->getRawSource();
	unsigned align = in->getAlignment();

	GlobalVariable *gv = dyn_cast<GlobalVariable>(src);
	if (gv && gv->isConstant()==true && gv->hasUnnamedAddr()==true && gv->hasPrivateLinkage()) {
		/*if (ConstantDataSequential *c = dyn_cast<ConstantDataSequential>(gv->getInitializer())) {
			if (c->isCString()) {
				return false;
			}
		}*/
		// eliminate privat global variable
		splitAggregateConstant(in, gv->getInitializer(), dest);
		removeVal.insert(gv);
		++NumElimGV;
	} else {
		// eliminate memcpy
		if (justGV) return false;
		Value* newVal = new LoadInst(/*ptr*/src, /*name*/"", /*volatile*/false, align, /*before*/in);
		new StoreInst(/*value*/newVal, /*ptr*/dest, /*volatile*/false, align, /*before*/in);

		if (rawsrc->getNumUses() == 1 && isa<Instruction>(rawsrc)) {
			removeVal.insert(rawsrc);
		}
	}

	if (rawdest->getNumUses() == 1 && dyn_cast<Instruction>(rawdest)) {
		removeVal.insert(rawdest);
	}
	return true;
}

// replace memset with zero or null or zeroinitializer
void LowerMemIntrinsicPass::replaceMemSet(MemSetInst *in) {

	auto dest = in->getDest();
	auto rawdest = in->getRawDest();

	// eliminate memset
	splitAggregateConstant(in, Constant::getNullValue(dest->getType()->getPointerElementType()), dest);

	if (rawdest->getNumUses() == 1 && dyn_cast<Instruction>(rawdest)) {
		removeVal.insert(rawdest);
	}

}

void LowerMemIntrinsicPass::splitAggregateConstant(Instruction *before, Constant *c, Value *ptr) {

	Type *cTy = c->getType();

	if (!cTy->isAggregateType()) { //simple constant
		new StoreInst(c, ptr, false, before);
		return;
	}

	// aggregate type
	unsigned numElms;
	if (isa<StructType>(cTy))
		numElms = cTy->getStructNumElements();
	else if (isa<ArrayType>(cTy))
		numElms = cTy->getArrayNumElements();
	else {
		// assert(false && "Unsupported type for aggregate constant.\n");
		errs() << getPassName() << ": Error: Unsupported type for aggregate constant \'" 
		       << *c << "\'\n";
		new StoreInst(c, ptr, false, before); // insert not change
		return;
	}

	LLVMContext &cxt = before->getParent()->getContext();
	Value *idx[2];
	idx[0] = ConstantInt::get(cxt, APInt(/*bits*/ 32, 0));

	for (unsigned i = 0; i < numElms; ++i) {
	
		Constant *elmVal = c->getAggregateElement(i);
		idx[1] = ConstantInt::get(cxt, APInt(/*bits*/ 32, i));
		Type *elmTy = elmVal->getType();
		Value *elmPtr = GetElementPtrInst::CreateInBounds(cTy, ptr, ArrayRef<Value*>(idx), "", before);

		if (elmTy->isAggregateType()) { // recursion
			splitAggregateConstant(before, elmVal, elmPtr);
		} else {
			new StoreInst(/*value*/ elmVal,/*ptr*/ elmPtr, /*isVolatile*/false, before);
		}
	}
}

void LowerMemIntrinsicPass::printMemCpyInst(MemCpyInst *in) {

	auto dest = in->getDest();
	auto src = in->getSource();
	auto rawdest = in->getRawDest();
	auto rawsrc = in->getRawSource();
	unsigned align = in->getAlignment();

	errs() << "S------------------------------------------------\n";
	errs() << "dest: "; dest->dump();
	errs() << "src: "; src->dump();
	errs() << "rawdest: "; rawdest->dump();
	errs() << "rawsrc: "; rawsrc->dump();
	if (in->getDest()->getType()->getPointerElementType() == in->getSource()->getType()->getPointerElementType())
		errs() << "\tOK\n"; // INAK NEJDE ELIMINOVAT
	errs() << "len: "; //uint64_t
	if (ConstantInt *len = dyn_cast<ConstantInt>(in->getLength())) {
		errs() << len->getValue().getZExtValue();
	} else { // NEJDE ELIMINOVAT
		errs() << "-";
	}
	// types are eq
	errs() << " / " << DL->getTypeSizeInBits(dest->getType()->getPointerElementType())/8 << "\n";
	errs() << "align: " << in->getAlignment() << " / ";

	if (align != 0 && align != 1) { // 0/1 => no alignment
		if (GlobalVariable *gv = dyn_cast<GlobalVariable>(dest)) {
			errs() << gv->getAlignment();
		} else if (AllocaInst *a = dyn_cast<AllocaInst>(dest)) {
			errs() << a->getAlignment();
		} else { // must be pointer
			errs() << /*src->getAlignment() <<*/ "?";
		}
		errs() << "(dest) / ";
		if (GlobalVariable *gv = dyn_cast<GlobalVariable>(src)) {
			errs() << gv->getAlignment();
		} else if (AllocaInst *a = dyn_cast<AllocaInst>(src)) {
			errs() << a->getAlignment();
		} else { // must be pointer
			errs() << /*src->getAlignment() <<*/ "?";
		}
		errs() << "(src)\n";
	} else errs() << "no align\n";


	if (isa<ConstantExpr>(rawdest)) {
		if (dest->getNumUses() == 1) {
			errs() << "remove-dest: "; dest->dump();
		}
	} else {
		if (rawdest->getNumUses() == 1) {
			errs() << "remove-rawdest: "; rawdest->dump();
		}
	}
	if (isa<ConstantExpr>(rawsrc)) {
		if (src->getNumUses() == 1) {
			errs() << "remove-src: "; src->dump();
		}
	} else {
		if (rawsrc->getNumUses() == 1) {
			errs() << "remove-rawsrc: "; rawsrc->dump();
		}
	}

	errs() << "PERFECTO\n";
	errs() << "E------------------------------------------------\n";
}


bool LowerMemIntrinsicPass::runOnModule(Module &M) {

#ifdef DEBUG
	errs() << "LOWER-MEM-MODIFICATION\n";
#endif
	bool changeEC = false;

	SmallPtrSet<Function*, 4> removeFunc; // if replace all uses

	for (Function &F : M)
		for (BasicBlock &BB : F)
			for (BasicBlock::iterator I = BB.begin(), IE = BB.end(); I != IE; ++I) {

				if (MemSetInst *in = dyn_cast<MemSetInst>(I)) {
				//@llvm.memset : i8* <dest>, i8 <val>, i64 <len>, i32 <align>, i1 <isvolatile>
#ifdef DEBUG
					in->dump();
#endif
					if (justGV || !testMemSet(in))
						continue;
					replaceMemSet(in);

					changeEC = true;

					removeFunc.insert(in->getCalledFunction());
					// edit iterator
					I = in->eraseFromParent();
					++NumElimMSet;

				} else if (MemCpyInst *in = dyn_cast<MemCpyInst>(I)) {
				//@llvm.memcpy : i8* <dest>, i8* <src>, i64 <len>, i32 <align>, i1 <isvolatile>
#ifdef DEBUG
					in->dump();
					printMemCpyInst(in);
#endif
					if (!testMemTransfer(in))
						continue;
					if (!replaceMemTransfer(in))
						continue;

					changeEC = true;

					removeFunc.insert(in->getCalledFunction());
					// edit iterator
					I = in->eraseFromParent();
					++NumElimMCpy;
				}

			}

	for (auto fnc = removeFunc.begin(), e = removeFunc.end(); fnc != e; ++fnc) {
		if ((*fnc)->getNumUses() == 0)
			(*fnc)->eraseFromParent();
	}
	for (auto val = removeVal.begin(), e = removeVal.end(); val != e; ++val) {
			if (isa<GlobalVariable>(*val))
				cast<GlobalVariable>(*val)->eraseFromParent();
			else if (isa<Instruction>(*val))
				cast<Instruction>(*val)->eraseFromParent();
			else
				assert(false && "Value not been erased.\n");
				//errs() << getPassName() << ": Error: Value \'" << (*val)->getName()
				//       << "\' not been erased.\n";
	}

	return changeEC;
}


bool LowerMemIntrinsicPass::doFinalization (Module &M) {

#ifdef DEBUG
	errs() << "LOWER-MEM-END\n";
#endif
	return true;
}

// opt pass registration
char LowerMemIntrinsicPass::ID = 0;
static RegisterPass<LowerMemIntrinsicPass> X("lowermem", "Lower memory intrinsic");

#undef  DEBUG_TYPE
