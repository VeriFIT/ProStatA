#ifndef H_NAMEBLOCKS_H
#define H_NAMEBLOCKS_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"

class NameBlocksPass : public llvm::FunctionPass {

    public:
        static char ID;
        NameBlocksPass() : llvm::FunctionPass(ID) {}
        bool runOnFunction(llvm::Function& f) override;

    private:
        llvm::Module *mod;
        size_t bbUID = 0;
};

#endif
