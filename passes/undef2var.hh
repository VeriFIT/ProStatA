#ifndef H_UNDEF2VAR_H
#define H_UNDEF2VAR_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"

class Undef2VarPass : public llvm::FunctionPass {
public:
    static char ID;

    Undef2VarPass() : FunctionPass(ID) {}
    bool runOnFunction(llvm::Function&) override;

private:
    bool processInstruction(llvm::Instruction& inst);
    llvm::Value* createVar(llvm::Function* func, llvm::Type* ty);
};

#endif /* end of include guard: H_UNDEF2VAR_H */
