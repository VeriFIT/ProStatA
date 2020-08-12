#include "elliminate_undefined.hh"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

using namespace llvm;

static const char* getNondetFuncName(const Type* type);
static Value *getOrInsertFunc(Module *module, Type *type, const char* name);

bool ElliminateUndefinedPass::runOnModule(Module& module) {
    bool changed = false;
    mod = &module;

    for (auto& func : module.functions()) {
        for (auto& BB : func) {
            for (auto& ins : BB) {
                for (auto& op : ins.operands()) {
                    auto* val = op.get();
                    if (isa<UndefValue>(val)) {
                        elliminateInInstruction(ins, op);
                        changed = true;
                    }
                }
            }
        }
    }

    return changed;
}

void ElliminateUndefinedPass::elliminateInInstruction(Instruction& ins,
                                                      Use& use) const {
    auto* val = cast<UndefValue>(use.get());
    auto* type = val->getType();
    const char* nondetFuncName = getNondetFuncName(type);
    assert(nondetFuncName != nullptr);

    auto* funcType = FunctionType::get(type, false);
    Value *func = getOrInsertFunc(mod, type, nondetFuncName);
    auto* call = CallInst::Create(funcType, func, nondetFuncName);
    use.set(call);
    call->insertBefore(&ins);
}

static Value *getOrInsertFunc(Module *module, Type *type, const char* name) {
#if LLVM_VERSION_MAJOR < 10
    return module->getOrInsertFunction(name, type);
#else
    return module->getOrInsertFunction(name, type).getCallee();
#endif
}

static const char* getNondetFuncName(const Type* type) {
    const char* name;

    if (type->isIntegerTy()) {
        if (type->getIntegerBitWidth() == 1) {
            return "__VERIFIER_nondet_bool";
        } else if (type->getIntegerBitWidth() == 8) {
            return "__VERIFIER_nondet_uchar";
        } else if (type->getIntegerBitWidth() == 16) {
            return "__VERIFIER_nondet_ushort";
        } else if (type->getIntegerBitWidth() == 32) {
            return "__VERIFIER_nondet_uint";
        } else if (type->getIntegerBitWidth() == 64) {
            return "__VERIFIER_nondet_ulong";
        } else {
            errs() << "warning: unsupported integer bitwidth "
                   << type->getIntegerBitWidth() << '\n';
        }
    } else if (type->isFloatTy()) {
        return "__VERIFIER_nondet_float";
    } else if (type->isDoubleTy()) {
        return "__VERIFIER_nondet_double";
    }

    return nullptr;
}


char ElliminateUndefinedPass::ID = 0;
static RegisterPass<ElliminateUndefinedPass> X(
    "elliminate-undefined",
    "Replace undefined values by nondeterministic values",
    false /* Only looks at CFG */, false /* Analysis Pass */);

static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder& Builder,
                                   legacy::PassManagerBase& PM) {
                                    PM.add(new ElliminateUndefinedPass());
                                });
