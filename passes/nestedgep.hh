#ifndef H_CONSTGEP_H
#define H_CONSTGEP_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"

class NestedGepPass : public llvm::FunctionPass {

    public:
        static char ID;
        NestedGepPass() : llvm::FunctionPass(ID) {}
        bool runOnFunction(llvm::Function& f) override;

    private:
        llvm::Module *mod;

        llvm::Value *processOperand(llvm::Value *value);
        void processOperandList(llvm::User::op_range ops);
        llvm::Value *tryReplaceSub(llvm::BinaryOperator *sub);
        llvm::Value *tryReplaceZExt(llvm::CastInst *ci);
        llvm::Value *tryReplaceSExt(llvm::CastInst *ci);
        llvm::Value *tryReplacePtrToInt(llvm::PtrToIntInst *pti);
        llvm::Value *tryReplace(llvm::ConstantExpr *ce);
};

#endif
