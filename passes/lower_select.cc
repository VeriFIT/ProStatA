/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate SelectInst -> create true and false branches
 * 
 * TODO: copy dbg info
 */

#include "lower_select.hh"
#include "passes.hh"

#include <cstdint> // __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "lowerselect"

STATISTIC(NumElim, "Number of eliminated select");

// interface
FunctionPass *createLowerSelectPass() {
  return new LowerSelectPass();
}

bool LowerSelectPass::doInitialization(Module &M) {
#ifdef DEBUG
	errs() << "LOWER-SELECT-START\n";
#endif
	DL = new DataLayout(&M);
	return true;
}

// split BB -> after select, create bb for true and false branch
void LowerSelectPass::replaceSelect(SelectInst *in) {
	
#ifdef DEBUG
	errs() << "S-------------------------------------\n";
	errs() << "cond: "; in->getCondition()->dump();
	errs() << "true: "; in->getTrueValue()->dump();
	errs() << "false: "; in->getFalseValue()->dump();
	errs() << "E-------------------------------------\n";
#endif
	
	BasicBlock *bbThis = in->getParent();
	Function *fnc = in->getParent()->getParent();
	unsigned align = DL->getPrefTypeAlignment(in->getType());

	// split at the select instruction and remove connection branch
	BasicBlock *bbEnd = bbThis->splitBasicBlock(in, "AfterSelect");
	bbThis->getTerminator()->eraseFromParent();

	BasicBlock *bbTrue = BasicBlock::Create(in->getContext(), "True", /*parent*/fnc, /*before*/bbEnd);
	BasicBlock *bbFalse = BasicBlock::Create(in->getContext(), "False", fnc, bbEnd);

	Value *newPtr = new AllocaInst(in->getType(), "", /*before*/bbThis->getFirstNonPHI());
	Instruction *before = BranchInst::Create(/*IfTrue*/bbTrue, /*IfFalse*/bbFalse, 
	                    /*Cond*/in->getCondition(), /*InsertAtEndBB*/bbThis);
	
	before = BranchInst::Create(bbEnd, /*InsertAtEndBB*/bbTrue);
	new StoreInst(/*value*/in->getTrueValue(), /*ptr*/newPtr, 
	              /*volatile*/false, align, /*before*/before);

	before = BranchInst::Create(bbEnd, /*InsertAtEndBB*/bbFalse);
	new StoreInst(/*value*/in->getFalseValue(), /*ptr*/newPtr, 
	              /*volatile*/false, align, /*before*/before);

	Value* newVal = new LoadInst(/*ptr*/newPtr, /*name*/"", /*volatile*/false, 
	              align, /*before*/bbEnd->getFirstNonPHI());
	in->replaceAllUsesWith(newVal);

	in->eraseFromParent();
	++NumElim;
}

bool LowerSelectPass::runOnFunction(Function &F) {
#ifdef DEBUG
	errs() << "LOWER-SELECT-MODIFICATION: " << F.getName() << "\n";
#endif
	bool changeEC = false;

	SmallPtrSet<Function*, 4> removeFunc; // if replace all uses

	for (Function::iterator BB = F.begin(), BE = F.end(); BB != BE; ++BB)
		for (BasicBlock::iterator I = BB->begin(), IE = BB->end(); I != IE; ++I) {
			if (SelectInst *in = dyn_cast<SelectInst>(I)) {
				replaceSelect(in);
				changeEC = true;
				break; // after split, next bb
			}
		}
	return changeEC;
}


bool LowerSelectPass::doFinalization (Module &M) {
#ifdef DEBUG
	errs() << "LOWER-SELECT-END\n";
#endif
	return true;
}

// opt pass registration
char LowerSelectPass::ID = 0;
static RegisterPass<LowerSelectPass> X("lowerselect", "Lower select");

#undef  DEBUG_TYPE
