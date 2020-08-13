#include "undef2var.hh"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include <memory>

using namespace llvm;

Value* Undef2VarPass::createVar(Function* func, Type* ty) {
    auto& ctx = func->getContext();

    auto dl = std::unique_ptr<DataLayout>(new DataLayout(func->getParent()));
    auto* first = func->getEntryBlock().getFirstNonPHI();
    return new AllocaInst(ty, dl->getAllocaAddrSpace(), "", first);
}

bool Undef2VarPass::processInstruction(Instruction& inst) {
    bool change = false;

    auto ops = make_range(inst.op_begin(), inst.op_end());
    for (auto& use : ops) {
        auto* val = use.get();
        if (isa<UndefValue>(val) && val->getType()->isSingleValueType()) {
            auto* var = createVar(inst.getFunction(), val->getType());
            auto* load = new LoadInst(val->getType(), var, "", &inst);
            use.set(load);
            change = true;
        }
    }

    return change;
}

bool Undef2VarPass::runOnFunction(Function& func) {
    bool change = false;

    for (auto& bb : func) {
        for (auto& inst : bb) {
            change |= processInstruction(inst);
        }
    }

    return change;
}

char Undef2VarPass::ID = 0;
static RegisterPass<Undef2VarPass> X("undef2var", "Replace scalar undef operands of instructions by uninitialized stack-allocated variables",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);
