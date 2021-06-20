//===-- cfgPrintPass.cpp ---------------------------------------*- C++ -*-===//
/* MIT License

    Copyright (c) 2021 Michael Canesche, Caio Raposo

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
//===----------------------------------------------------------------------===//
/*
 \file
 This file contains the declaration of the analysis used to print all
 instructions as CFG.
*/
//===----------------------------------------------------------------------===//

#ifndef CFGPRINT_H
#define CFGPRINT_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "llvm/IR/PassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include <llvm/Support/raw_ostream.h>

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
        void BrNode(BasicBlock::iterator I, vector<string> &target);
        void PhiNode(BasicBlock::iterator I);
        void RetNode(BasicBlock::iterator I);
        void SExtNode(BasicBlock::iterator I);
        void CallNode(BasicBlock::iterator I);
        void StoreNode(BasicBlock::iterator I);
        void OtherNode(BasicBlock::iterator I);
        
        private:
            raw_ostream &OS;
            int counter;
            unordered_map<string, string> opInstr;
            ofstream outFile;
    };

} // namespace addconst

#endif