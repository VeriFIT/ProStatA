/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate SelectInst -> create true and false branches
 */

#ifndef H_LOWER_SELECT_H
#define H_LOWER_SELECT_H

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

struct LowerSelectPass : public FunctionPass {
		DataLayout *DL;

	public:
		static char ID;

		LowerSelectPass() : FunctionPass(ID), DL(nullptr) {}
		virtual bool doInitialization(Module &);
		virtual bool runOnFunction(Function &);
		virtual bool doFinalization(Module &);
	private:
		void replaceSelect(SelectInst *);
};

#endif /* H_LOWER_SELECT_H */
