/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate PHINode -> assignment moves to previous BasicBlock, 
 * new LLVM IR is in non-SSA form
 */

#include "elim_phi.hh"
#include "passes.hh"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// interface
FunctionPass *createEliminatePHIPass() {
	return new EliminatePHIPass();
}

bool EliminatePHIPass::doInitialization(Module &) {
#ifdef DEBUG
	errs() << "ELIM-PHI-START\n";
#endif
	return true;
}

/// test, if instruction in next BasicBlock is PHI
inline bool EliminatePHIPass::testPHI(BasicBlock *phiBB) {

	return isa<PHINode>(phiBB->begin());
}

// return corresponding variable for phi assign, if not in map:
// create new variable by alloca as 1st instruction in function
// in the place phi, load this new variable and replace all uses
Value *EliminatePHIPass::getAssign(PHINode *in) {

	auto item = PHIMap.find(in);
	if (item == PHIMap.end()) {
		// not found -> create new variable with alloca

		// 1st instruction in function
		Instruction *before = in->getParent()->getParent()->getEntryBlock().begin();
		// getFunction === getParent()->getParent() clang 3.8++
		Value *newPtr = new AllocaInst(in->getType(), "phi_ptr", /*before*/before);

		before = in->getParent()->getFirstNonPHI();
		Value* newVal = new LoadInst(/*ptr*/newPtr, /*name*/"phi_val", /*before*/before);
		in->replaceAllUsesWith(newVal);

		PHIMap.insert({in, newPtr});
		return newPtr;
	}

	return item->second;
}

// if unconditional branch: 
//       into BasicBlock (fromBB, witch called this function) 
//       added assign instruction with value for fromBB
// else  this instruction is added into new BasicBlock
void EliminatePHIPass::replacePHIAssign(BasicBlock *fromBB, BasicBlock *phiBB, BranchInst *branch, unsigned idx) {

	BasicBlock::iterator I = phiBB->begin();
	Instruction *before = branch;
	
	while (isa<PHINode>(I)) { // all phi nodes

		PHINode *phi = cast<PHINode>(I); ++I;

		Value *newAssign = getAssign(phi);
		Value *val = phi->removeIncomingValue(fromBB, /*deleteEmpty*/ true);


		if (branch->isConditional()) {
			BasicBlock *newBB = BasicBlock::Create(phi->getContext(), "PhiJump", 
			                    /*parent*/phiBB->getParent(), /*before*/phiBB);
			before = BranchInst::Create(phiBB, /*InsertAtEndBB*/newBB);

			// redirect jump in fromBB through newBB
			branch->setSuccessor(/*idx*/idx, /*bb*/newBB);
		}

		new StoreInst(/*value*/ val, /*ptr*/ newAssign, /*volatile*/false, /*before*/before);
	}
}

bool EliminatePHIPass::runOnFunction(Function &F) {

#ifdef DEBUG
	errs() << "LOWER-SELECT-MODIFICATION: " << F.getName() << "\n";
#endif
	bool changeEC = false;

	for (BasicBlock &BB : F) {
		for (Instruction &I : BB) {
			if (BranchInst *in = dyn_cast<BranchInst>(&I)) {

				BasicBlock *bb = in->getSuccessor(0);
				if (testPHI(bb)) {
					replacePHIAssign(in->getParent(), bb, in, 0);
					changeEC = true;
				}

				if (in->isConditional()) {
					bb = in->getSuccessor(1);
					if (testPHI(bb)) {
						replacePHIAssign(in->getParent(), bb, in, 1);
						changeEC = true;
					}
				}
			}
		}
	}

	return changeEC;
}

bool EliminatePHIPass::doFinalization (Module &) {
#ifdef DEBUG
	errs() << "ELIM-PHI-END\n";
#endif
	return true;
}

// opt pass registration
char EliminatePHIPass::ID = 0;
static RegisterPass<EliminatePHIPass> X("elim-phi", "Eliminate PhiNode");
