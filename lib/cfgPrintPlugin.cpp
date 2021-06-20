//===-- cfgPrintPlugin.cpp ---------------------------------------*- C++ -*-===//
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
    This file is the entry point for the New PM opt plugin. That is, it
    contains the New PM registration for all the analyses and transformations
    related to the cfgPrint plugin.
*/
//===----------------------------------------------------------------------===//

#include "cfgPrint.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

bool registerPipeline(StringRef Name, FunctionPassManager &FPM,
                      ArrayRef<PassBuilder::PipelineElement>) {
    
    if (Name == "cfgPrinter") {
        FPM.addPass(cfgPrint::cfgPrinterPass(errs()));
        return true;
    }

    return false;
}

PassPluginLibraryInfo getCFGConstPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "cfgPrinter", LLVM_VERSION_STRING, 
            [](PassBuilder &PB){
        PB.registerPipelineParsingCallback(registerPipeline);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getCFGConstPluginInfo();
}
