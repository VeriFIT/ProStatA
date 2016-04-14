/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate llvm.memset / llvm.memcpy / (llvm.memmove) if possible
 * create corresponding function
 * !!! don't copy padding
 */

#ifndef H_LOWER_MEM_H
#define H_LOWER_MEM_H

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IntrinsicInst.h"

using namespace llvm;

struct LowerMemIntrinsicPass : public ModulePass {
	protected:
		bool justGV;
		DataLayout *DL;
		SmallPtrSet<Value*, 8> removeVal;

	public:
		static char ID;

		LowerMemIntrinsicPass() : ModulePass(ID), 
		                          justGV(false), DL(nullptr) {}
		LowerMemIntrinsicPass(bool gv) : ModulePass(ID),
		                          justGV(gv), DL(nullptr) {}
		virtual bool doInitialization(Module &);
		virtual bool runOnModule(Module &);
		virtual bool doFinalization(Module &);

	private:
		bool testMem(MemIntrinsic *);
		bool testMemTransfer(MemTransferInst *);
		bool testMemSet(MemSetInst *);
		bool replaceMemTransfer(MemTransferInst *);
		void replaceMemSet(MemSetInst *);
//		void printMemCpy(MemTransferInst *);
};

#endif /* H_LOWER_MEM_H */
