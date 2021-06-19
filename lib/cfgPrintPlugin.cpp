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
        //PB.registerAnalysisRegistrationCallback(registerAnalyses);
        PB.registerPipelineParsingCallback(registerPipeline);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getCFGConstPluginInfo();
}