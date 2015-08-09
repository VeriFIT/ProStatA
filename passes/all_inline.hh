/** 
 * Veronika Sokova <xsokov00@stud.fit.vutbr.cz>
 * 
 * Flattening call-graph 
 * Each function inlined (required: without recursion)
 */

#ifndef H_ALL_INLINE_H
#define H_ALL_INLINE_H

#include "llvm/Transforms/IPO/InlinerPass.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/ADT/SmallPtrSet.h"

using namespace llvm;

class AllAsInlinePass : public Inliner {
		InlineCostAnalysis *ICA;
		SmallPtrSet<Function*, 8> funcNonInlined;

	public:
		static char ID;
		
		AllAsInlinePass() : Inliner(ID, -2000000000, /*InsertLifetime*/ false),
		                    ICA(nullptr) {}
		AllAsInlinePass(bool InsertLifetime) : Inliner(ID, -2000000000, InsertLifetime),
		                    ICA(nullptr) {}
		virtual void getAnalysisUsage(AnalysisUsage &AU) const {
			AU.addRequired<InlineCostAnalysis>();
			Inliner::getAnalysisUsage(AU);
		}

		virtual InlineCost getInlineCost(CallSite CS);
//		virtual bool doInitialization(CallGraph &); // add and remove functions
		virtual bool runOnSCC(CallGraphSCC &);
		virtual bool doFinalization(CallGraph &);
};

#endif /* H_ALL_INLINE_H */
