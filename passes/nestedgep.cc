#include "nestedgep.hh"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <vector>

using namespace llvm;

bool NestedGepPass::runOnFunction(Function &F) {
    mod = F.getParent();
    for (auto &bb : F) {
        for (auto &ins : bb) {
            if (auto *gep = dyn_cast_or_null<GetElementPtrInst>(&ins)) {
                processOperandList(gep->indices());
            }
        }
    }

    return true;
}

Value *NestedGepPass::processOperand(Value *value) {
    auto *cex = dyn_cast_or_null<ConstantExpr>(value);
    if (cex) {
        return tryReplace(cex);
    }

    return nullptr;
}

void NestedGepPass::processOperandList(User::op_range ops) {
    for (auto &use : ops) {
        auto *replacement = processOperand(use.get());
        if (replacement) use.set(replacement);
    }
}

Value *NestedGepPass::tryReplaceSub(BinaryOperator *sub) {
    processOperandList(sub->operands());

    auto *op1 = dyn_cast_or_null<ConstantInt>(sub->getOperand(0));
    auto *op2 = dyn_cast_or_null<ConstantInt>(sub->getOperand(1));

    if (!op1 || !op2) {
        return nullptr;
    }

    if (sub->hasNoSignedWrap()) {
        int64_t a = op1->getSExtValue();
        int64_t b = op2->getSExtValue();

        return ConstantInt::getSigned(sub->getType(), a - b);

    } else {
        int64_t a = op1->getZExtValue();
        int64_t b = op2->getZExtValue();

        return ConstantInt::get(sub->getType(), a - b);
    }
}

Value *NestedGepPass::tryReplaceZExt(CastInst *ci) {
    processOperandList(ci->operands());
    auto *op = cast<ConstantInt>(ci->getOperand(0));

    return ConstantInt::get(ci->getDestTy(), op->getZExtValue());
}

Value *NestedGepPass::tryReplaceSExt(CastInst *ci) {
    processOperandList(ci->operands());
    auto *op = cast<ConstantInt>(ci->getOperand(0));

    return ConstantInt::getSigned(ci->getDestTy(), op->getSExtValue());
}

Value *NestedGepPass::tryReplacePtrToInt(PtrToIntInst *pti) {
    if (!pti) return nullptr;

    auto *pto = pti->getPointerOperand();
    if (auto *ce = dyn_cast_or_null<ConstantExpr>(pto)) {
        auto *inst = ce->getAsInstruction();

        if (auto *gep = dyn_cast_or_null<GetElementPtrInst>(inst)) {
            auto &dl = mod->getDataLayout();
            auto *ptrType = gep->getPointerOperand()->getType();

            auto offset =
                APInt::getNullValue(dl.getTypeAllocSizeInBits(ptrType));
            if (gep->accumulateConstantOffset(dl, offset)) {
                return ConstantInt::get(pti->getType(), offset);
            }  // else the gep doesn't have constant offset and cannot be
               // replaced
        }      // else there is not gep as an operand. no idea what to do
    }

    return nullptr;
}

Value *NestedGepPass::tryReplace(ConstantExpr *ce) {
    if (!ce) return nullptr;

    auto *ins = ce->getAsInstruction();

    switch (ins->getOpcode()) {
        case Instruction::Sub:
            return tryReplaceSub(cast<BinaryOperator>(ins));
        case Instruction::ZExt:
            return tryReplaceZExt(cast<CastInst>(ins));
        case Instruction::SExt:
            return tryReplaceSExt(cast<CastInst>(ins));
        case Instruction::PtrToInt:
            return tryReplacePtrToInt(cast<PtrToIntInst>(ins));
        default:
            return nullptr;
    }
}

char NestedGepPass::ID = 0;
static RegisterPass<NestedGepPass> X("nestedgep", "Evaluate inner GetElementPtr",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);
