#ifndef ADDCONST_H
#define ADDCONST_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "llvm/IR/PassManager.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;
using namespace std;

namespace cfgPrint {

    struct cfgPrinterPass : public PassInfoMixin<cfgPrinterPass> {

        explicit cfgPrinterPass(raw_ostream &OS) : OS(OS) {}
        
        PreservedAnalyses run(Function &F,
                              FunctionAnalysisManager &FAM);
        
        void runBasicBlock(Function::iterator &bb);
        void runInstruction(BasicBlock::iterator it, vector<string> &target);

        string getLabel(const Value *v);
        string getName(const Value *v); 

        void get_map_label(Function &F);
        
        private:
            raw_ostream &OS;
            int counter;
            unordered_map<string, string> opInstr;
            ofstream outFile;
    };

} // namespace addconst

#endif