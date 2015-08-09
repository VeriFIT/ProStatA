/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Flattening call-graph 
 * Each function inlined (required: without recursion)
 */

#include "all_inline.hh"
#include "passes.hh"

#include <cstdint> // __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// interface
Pass *createAllAsInlinePass(bool InsertLifetime) {
	return new AllAsInlinePass(InsertLifetime);
}

InlineCost AllAsInlinePass::getInlineCost(CallSite CS) {
	Function *Callee = CS.getCalledFunction();
	if (Callee && !Callee->isDeclaration() &&
	ICA->isInlineViable(*Callee))
		return InlineCost::getAlways();
	// don't body
	if (!Callee->isDeclaration())
		funcNonInlined.insert(Callee);
	return InlineCost::getNever();
}

bool AllAsInlinePass::runOnSCC(CallGraphSCC &SCC) {
#ifdef DEBUG
	errs() << "ALL-INLINE-MODIFICATION-SCC\n";
#endif
	ICA = &getAnalysis<InlineCostAnalysis>();
	return Inliner::runOnSCC(SCC);
}

bool AllAsInlinePass::doFinalization(CallGraph &CG) {
#ifdef DEBUG
	errs() << "ALL-INLINE-END\n";
#endif
	for (SmallPtrSetImpl<Function *>::iterator I = funcNonInlined.begin(),
			E = funcNonInlined.end(); I != E; ++I) {
		errs() << getPassName() << ": Warning: Function \'" << (*I)->getName()
		       << "\' not inlined.\n";
	}
	funcNonInlined.clear();

	return Inliner::removeDeadFunctions(CG, /*AlwaysInlineOnly*/false);
}


// opt pass registration
char AllAsInlinePass::ID = 0;
static RegisterPass<AllAsInlinePass> X("all-inline", "Flattening call-graph");
