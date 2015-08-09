/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Test ~analyzer~ as clang plug-in
 * clang -Xclang -load -Xclang ../passes_build/libtest.so source.c
 */


#include "passes.hh" // createGlobalVarsPass...
#include "llvm/Transforms/Scalar.h" // createLowerSwitchPass
#include "llvm/Transforms/IPO.h" // createAlwaysInlinerPass

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

struct TestPass : public ModulePass {
	public:
		static char ID;

		TestPass() : ModulePass(ID) {}
//		virtual bool doInitialization(Module &);
		virtual bool runOnModule(Module &M); // must override
		virtual bool doFinalization(Module &);
};

bool TestPass::runOnModule(Module &M) {
#ifdef DEBUG
	errs() << "TEST-USERS-MODIFICATION\n";
#endif
	/*
	for (Function &F : M)
		F.addFnAttr(Attribute::AlwaysInline);
	*/
	return false;
}

bool TestPass::doFinalization(Module &M) {
#ifdef DEBUG
	errs() << "TEST-RUN-ANALYZER\n";
#endif
	return true;
}

// registraction opt LLVM pass
char TestPass::ID = 0;
static RegisterPass<TestPass> X("test", "Testing something");

// registraction clang plug-in
static void registerTestPass(const PassManagerBuilder &pb,
                         legacy::PassManagerBase &pm) 
{
	if (pb.OptLevel == 0 && pb.SizeLevel == 0) {
//		pm.add(createLowerSwitchPass()); // -lowerswitch
//		pm.add(createLowerSelectPass()); // -lowerselect
//		pm.add(createPromoteMemoryToRegisterPass()); // -mem2reg
//		pm.add(createGlobalVarsPass()); // -global-vars
//		pm.add(createLowerMemIntrinsicPass(/*justGV*/false)); // -lowermem
//		pm.add(createAllAsInlinePass()); // -all-inline
		pm.add(new TestPass);
//		pm.add(createAlwaysInlinerPass(false)); // -always-inline
	}
}

static RegisterStandardPasses
RegisterTestPass(PassManagerBuilder::EP_EnabledOnOptLevel0, registerTestPass);
