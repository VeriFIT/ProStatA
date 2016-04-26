/** 
 * Copyright (C) 2015-2016 Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Interface for registration LLVM passes
 */

#ifndef H_PASSES_H
#define H_PASSES_H

#include "llvm/Pass.h"

// Global variables initialization in new function __initGlobalVar()
llvm::ModulePass *createGlobalVarsPass();

// Eliminate llvm.memcpy / llvm.memmove / llvm.memset -> create 
// corresponding function (eliminate local constant)
// Output: don't copy padding
llvm::ModulePass *createLowerMemIntrinsicPass(bool justGV=false);

// Flattening call-graph -> each function inlined
// Input: without recursion
llvm::Pass *createAllAsInlinePass(bool InsertLifetime=false);

// Eliminate SelectInst -> create true and false branches
llvm::FunctionPass *createLowerSelectPass();

// Eliminate PHINode -> assignment moves to previous BasicBlock
// if LowerSelectPass, call after it
// Output: not SSA form
llvm::FunctionPass *createEliminatePHIPass();

// Eliminate ConstantExpr as operand -> create as instruction and
// operand is replaced with result of instruction
// if GlobalVarsPass, call after it
// Output: don't eliminate CEs in global variables (not apply with GlobalVarsPass)
llvm::BasicBlockPass *createEliminateConstExprPass();

#endif /* H_PASSES_H */
