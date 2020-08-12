#ifndef ELLIMINATE_UNDEFINED_HPP_WSIRHVJZ
#define ELLIMINATE_UNDEFINED_HPP_WSIRHVJZ

#include <llvm/Pass.h>
#include <llvm/IR/Instruction.h>

class ElliminateUndefinedPass : public llvm::ModulePass {
public:
    static char ID;

    ElliminateUndefinedPass() : llvm::ModulePass(ID) {}

    bool runOnModule(llvm::Module& module) override;

private:
    llvm::Module* mod = nullptr;

    void elliminateInInstruction(llvm::Instruction& instruction, llvm::Use& use) const;
};

#endif /* end of include guard: ELLIMINATE_UNDEFINED_HPP_WSIRHVJZ */
