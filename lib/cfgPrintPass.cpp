#include "cfgPrint.h"

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
        int opcode = I->getOpcode();
        
        switch (opcode) {
            case Instruction::Br: // Branch
                BrNode(I, target); 
                break;
            case Instruction::PHI: // PHINODE
                PhiNode(I); 
                break;
            case Instruction::Ret:
                RetNode(I); 
                break;
            case Instruction::SExt:
                break;
            default:
                OtherNode(I); 
                break;
        }
    }

    void cfgPrinterPass::BrNode(BasicBlock::iterator I, vector<string> &target) {
        outFile << " " << I->getOpcodeName() << " ";

        for (int i = 0; i < I->getNumOperands(); ++i) {
            Value *v = I->getOperand(i);
            
            if (i > 0 || I->getNumOperands() == 1) {
                outFile << " BB" << getName(v);
                target.push_back(getName(v));
            } else {
                outFile << "%" << getName(v); 
            }
        }
        outFile << "\\l"; 
    }

    void cfgPrinterPass::PhiNode(BasicBlock::iterator I) {
        
        PHINode *phi = dyn_cast<PHINode>(I);
        outFile << " %" << getLabel(&(*I)) << " = "  << phi->getOpcodeName() << " ";
        
        for (int i = 0; i < phi->getNumIncomingValues(); ++i) {
            outFile << "[ ";
            if (isa<Constant>(phi->getIncomingValue(i))) {
                outFile << dyn_cast<ConstantInt>(phi->getIncomingValue(i))->getZExtValue();
            } else {
                outFile << " %" << getName(phi->getIncomingValue(i));
            }
            outFile << ", BB" << getName(phi->getIncomingBlock(i)) << " ]";

            if (i == 0) outFile << ", ";
        }
        outFile << "\\l";
    }

    void cfgPrinterPass::RetNode(BasicBlock::iterator I) {
        outFile << " " << I->getOpcodeName() << " " << getLabel(&(*I)) << "\\l";
    }

    void cfgPrinterPass::OtherNode(BasicBlock::iterator I) {
        outFile << " %" << getName(&(*I)) << " = " << I->getOpcodeName();
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
                //errs() << getLabel(*op) << " ";
                outFile << " %" << getName(*op);
                if (I->getNumOperands()-1 != c) outFile << ", ";
                c++;
            }
        }
        outFile << "\\l";
    }
}
