#include "nameblocks.hh"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

using namespace llvm;

char NameBlocksPass::ID = 0;
static RegisterPass<NameBlocksPass> X("nameblocks",
                                      "Name all blocks in program",
                                      false /* Only looks at CFG */,
                                      false /* Analysis Pass */);

bool NameBlocksPass::runOnFunction(Function& f) {
    bool changed = false;
    for (auto& BB : f) {
        if (!BB.hasName()) {
            std::string label = std::string("<label") + std::to_string(bbUID) + ">";
            BB.setName(label);
            ++bbUID;
            changed = true;
        }
    }

    return changed;
}
