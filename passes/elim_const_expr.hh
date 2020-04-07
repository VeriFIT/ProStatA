/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Eliminate ConstantExpr as operand -> create as instruction and
 * operand is replaced with result of instruction
 * Output: don't eliminate CEs in global variables (not apply with GlobalVarsPass)
 */

#ifndef H_ELIM_CONST_EXPR_H
#define H_ELIM_CONST_EXPR_H

#include "llvm/Pass.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/BasicBlock.h"

using namespace llvm;

struct EliminateConstExprPass : public FunctionPass {

	public:
		static char ID;

		EliminateConstExprPass() : FunctionPass(ID) {}
		bool doInitialization(Module &) override;
		bool runOnFunction(Function &) override;
		bool doFinalization(Module &) override;

	private:
		bool testConstExpr(Value *);
		void replaceConstExpr(ConstantExpr *c, unsigned, Instruction *from);
		bool findConstExpr(Instruction *);
		bool findConstExprForCall(CallInst *);
};

#endif /* H_ELIM_CONST_EXPR_H */
