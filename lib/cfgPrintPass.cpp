#include "cfgPrint.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"

using namespace llvm;

namespace cfgPrint {

    string cfgPrinterPass::getLabel(const Value *v) {
        StringRef bbName(v->getName());
        return bbName.str();
    }

    string cfgPrinterPass::getName(const Value *v) {
        string s = getLabel(v);
        if (opInstr.count(s) > 0) {
            return opInstr[s];
        }
        return s;
    }

    void cfgPrinterPass::get_map_label(Function &F) {
        for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
            opInstr[getLabel(&(*bb))] = to_string(counter++);
            errs() << opInstr[getLabel(&(*bb))] << "BB\n";
            for (BasicBlock::iterator I = bb->begin(), e = bb->end(); I != e; ++I) {
                if (isa<SExtInst>(*I)) {
                    opInstr[getLabel(&(*I))] = getLabel(I->getOperand(0));
                } else if (!isa<BranchInst>(*I) && !isa<PHINode>(*I) && !isa<ReturnInst>(*I)) {
                    opInstr[getLabel(&(*I))] = to_string(counter++);
                    errs() << opInstr[getLabel(&(*I))] << "I\n";
                }
            }
        }
    }

    PreservedAnalyses cfgPrinterPass::run(Function &F, FunctionAnalysisManager &FAM) {

        // Counter for Instruction and branch
        counter = 0;

        outFile.open(F.getName().str()+".dot");

        get_map_label(F);

        outFile << "digraph \"CFG for '" << F.getName().str() << "' function\" {\n";
        for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
            runBasicBlock(bb);
        }
        outFile << "}\n";

        return PreservedAnalyses::all();
    }

    void cfgPrinterPass::runBasicBlock(Function::iterator &bb) {

        string block_name = getName(&(*bb)); 
        
        outFile << "\tAA" << block_name <<" [shape=record, label=\"{";
        outFile << "BB" << block_name << ":\\l\\l ";
        
        vector<string> target;
        for (BasicBlock::iterator I = bb->begin(), e = bb->end(); I != e; ++I) {
            runInstruction(I, target);
        }
        outFile << "}\"];\n";

        for (vector<string>::iterator it = target.begin(), end = target.end(); it != end; ++it) {
            outFile << "\tAA" << block_name << " -> AA" << *it << "\n";
        }
    }

    void cfgPrinterPass::runInstruction(BasicBlock::iterator I, vector<string> &target) {

        // get the opcode name of each instruction
        string opcodeName = I->getOpcodeName();
        
        
        /*int opcode = I->getOpcode();
        switch (opcode) {
        case :
            // code 
            break;
        
        default:
            break;
        }*/

        if (isa<BranchInst>(*I)) { // branch instruction
            outFile << " " << opcodeName << " ";
            int number_operands = I->getNumOperands();

            for (int i = 0; i < number_operands; ++i) {
                Value *v = I->getOperand(i);
               
                if (i > 0 || number_operands == 1) {
                    outFile << " BB" << getName(v);
                    target.push_back(getName(v));
                } else {
                    outFile << "%" << getName(v); 
                }
            } 
        } else if (isa<PHINode>(*I)) { // PHI code
            PHINode *phi = dyn_cast<PHINode>(I);

            outFile << " %" << getLabel(&(*I)) << " = "  << phi->getOpcodeName() << " ";

            for (int i = 0; i < phi->getNumIncomingValues(); ++i) {
                
                outFile << "[ ";
                Value *v0 = phi->getIncomingValue(i);

                if (isa<Constant>(v0)) {
                    outFile << dyn_cast<ConstantInt>(v0)->getZExtValue();
                } else {
                    outFile << " %" << getName(v0);
                }
                outFile << ", ";

                v0 = phi->getIncomingBlock(i);
                outFile << "BB" << getName(v0) << "]";

                if (i == 0) outFile << ", ";
            }
        } else if (isa<ReturnInst>(*I)) {
            outFile << " " << opcodeName << " " << getLabel(&(*I));
        } else if (isa<SExtInst>(*I)) {

        } else {

            outFile << " %" << getName(&(*I)) << " = " << opcodeName;
            
            if (isa<ICmpInst>(*I)) {
                ICmpInst *ICC = dyn_cast<ICmpInst>(I);
                llvm::CmpInst::Predicate pr = ICC->getSignedPredicate();
                switch(pr){
                    case CmpInst::ICMP_SGT: outFile << "-sgt "; break;
                    case CmpInst::ICMP_SLT: outFile << "-slt "; break; 
                    case CmpInst::ICMP_SGE: outFile << "-sge "; break; 
                    case CmpInst::ICMP_SLE: outFile << "-sle "; break;
                }
            }
            
            int c = 0;
            for (User::op_iterator op = I->op_begin(), e = I->op_end(); op != e; op++) {
                if (getLabel(*op).size() > 0) {
                    errs() << getLabel(*op) << " ";
                    outFile << " %" << getName(*op);
                    if (I->getNumOperands()-1 != c) outFile << ", ";
                    c++;
                }
            }
            errs() << "\n";
        }
        
        outFile << "\\l";
    }
}
