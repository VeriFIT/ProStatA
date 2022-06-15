/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate ConstantExpr as operand -> create as instruction and
 * operand is replaced with result of instruction
 * Output: don't eliminate CEs in global variables (not apply with GlobalVarsPass)
 */

#include "elim_const_expr.hh"
#include "passes.hh"

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// interface
FunctionPass *createEliminateConstExprPass() {
	return new EliminateConstExprPass();
}

bool EliminateConstExprPass::doInitialization(Module &) {
#ifdef DEBUG
	errs() << "ELIM-CONST-EXPR-START\n";
#endif
	return true;
}

inline bool EliminateConstExprPass::testConstExpr(Value *v) {
	return isa<ConstantExpr>(v);
}

// create new instruction from constant expression and all its uses 
// are replaced with result of instruction
// try to find constant expression in new instruction
void EliminateConstExprPass::replaceConstExpr(ConstantExpr *ce, unsigned idx, Instruction *from) {

#ifdef DEBUG
	ce->dump();
#endif
	Instruction *newInst = ce->getAsInstruction();
	
	if (PHINode *phi = dyn_cast<PHINode>(from)) {
		BasicBlock *bb = phi->getIncomingBlock(idx);
		newInst->insertBefore(bb->getTerminator());
	} else {
		newInst->insertBefore(from);
	}
	from->replaceUsesOfWith(/*from*/ce, /*to*/newInst);
	findConstExpr(newInst); // indirect recursion
}


// check all operands, find ConstantExpr and replace it as instruction
// return true, if find some ConstantExpr
bool EliminateConstExprPass::findConstExpr(Instruction *in) {

	bool find = false;
	for (unsigned i=0, ops = in->getNumOperands(); i < ops ; ++i) {
		Value *op = in->getOperand(i);
		if (testConstExpr(op)) {
			replaceConstExpr(cast<ConstantExpr>(op), i, in); // indirect recursion
			find = true;
		}
	}
	return find;
}

// for call instructions
// check all arguments, find ConstantExpr and replace it as instruction
// return true, if find some ConstantExpr
bool EliminateConstExprPass::findConstExprForCall(CallInst *in) {

	bool find = false;
#if LLVM_VERSION_MAJOR >= 8
	for (unsigned i=0, ops = in->arg_size(); i < ops ; ++i) {
#else
	for (unsigned i=0, ops = in->getNumArgOperands(); i < ops ; ++i) {
#endif
		Value *op = in->getArgOperand(i);
		if (testConstExpr(op)) {
			replaceConstExpr(cast<ConstantExpr>(op), i, in); // indirect recursion
			find = true;
		}
	}
	return find;
}

bool EliminateConstExprPass::runOnFunction(Function &F) {

	bool changeEC = false;
#ifdef DEBUG
	errs() << "=================================================\n";
#endif

	for (BasicBlock &BB : F) {
		for (Instruction &I : BB) {
#ifdef DEBUG
			errs() << "----"<<I<<"\n";
#endif
			if (CallInst *call = dyn_cast<CallInst>(&I)) {
				changeEC = findConstExprForCall(call);
			} else {
				changeEC = findConstExpr(&I);
			}
		}
	}
#ifdef DEBUG
	errs() << "=================================================\n";
#endif
	return changeEC;
}


bool EliminateConstExprPass::doFinalization (Module &) {
#ifdef DEBUG
	errs() << "ELIM-CONST-EXPR-END\n";
#endif
	return true;
}

// opt pass registration
char EliminateConstExprPass::ID = 0;
static RegisterPass<EliminateConstExprPass> X("elim-const-expr", "Eliminate ConstantExpr as operand");
