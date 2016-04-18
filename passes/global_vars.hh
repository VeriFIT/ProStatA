/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * All global variables are implicitly initialized to 0 or nullptr.
 * Private "global" variables (StructType) are change to non-constant initalizer
 */

#ifndef H_GLOBAL_VARS_H
#define H_GLOBAL_VARS_H

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h" // insert code

using namespace llvm;

struct GlobalVarsPass : public ModulePass {
	public:
		static char ID;
		
		GlobalVarsPass() : ModulePass(ID) {}
//		virtual void getAnalysisUsage(AnalysisUsage &AU) const;
		using Pass::doInitialization;
		virtual bool doInitialization(Module &);
		virtual bool runOnModule(Module &);
		virtual bool doFinalization(Module &);
	private:
		void printLinkage(enum GlobalValue::LinkageTypes l);
		void printGlobalVar(GlobalVariable *GV);
		void splitAggregateConstant(IRBuilder<>*, Constant*, Value*);
};

#endif /* H_GLOBAL_VARS_H */
