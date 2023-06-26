//
// LLVM Constant Folding and Constant Propagation Pass
//  
// Created using the provided CS57 LLVM Skeleton which notes:
// 'Parts taken from skeleton Copyright (c) 2015 Adrian Sampson at 
// https://github.com/sampsyo/llvm-pass-skeleton/blob/master/skeleton/Skeleton.cpp
// License file included in directory.
//
// 01 May 2022  jpb  Creation from foundational works shown.'
//
// William Dinauer, Dartmouth CS57, Spring 2022
//

#include "llvm/Pass.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/NoFolder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"
#include "llvm/IR/LLVMContext.h"

#define DEBUG_TYPE "FoldPropPass"

namespace {

    // getInt function, from one of the provided CS57 skeleton passes
    int getInt(llvm::Value *val) {
        if(llvm::ConstantInt *CI = llvm::dyn_cast<llvm::ConstantInt>(val)) {
            return CI->getSExtValue();
        }
        return 0;
    }

  struct FoldPropPass : public llvm::FunctionPass {
    static char ID;
    FoldPropPass() : FunctionPass(ID) {}
    virtual bool runOnFunction(llvm::Function &F) {
      bool changes = false;
      for (auto &B : F) {                                               // For every Basic Block
        for (auto I = B.begin(); I != B.end(); I++) {                   // For every Instruction in the Basic Block
            if (auto *op = llvm::dyn_cast<llvm::BinaryOperator>(I)) {   // Check if the Instruction is a Binary Operation
                llvm::Value *lhs = op->getOperand(0);               
                llvm::Value *rhs = op->getOperand(1);
                if (llvm::isa<llvm::ConstantInt>(lhs) & llvm::isa<llvm::ConstantInt>(rhs)) {    // Check if the left and right operands are constants
                    // Store the result of the operation into res
                    int res = 0;
                    switch (op->getOpcode()) {
                        case llvm::Instruction::Add:            // '+'
                            res = getInt(lhs) + getInt(rhs);
                            break;
                        case llvm::Instruction::Sub:            // '-'
                            res = getInt(lhs) - getInt(rhs);
                            break;
                        case llvm::Instruction::Mul:            // '*'
                            res = getInt(lhs) * getInt(rhs);
                            break;
                        case llvm::Instruction::SDiv:           // '/'
                            res = getInt(lhs) / getInt(rhs);
                            break;
                        default:
                            llvm::errs() << "unknown operator\n";
                            break;
                    }
                    // Replace the all future uses of this instruction with the calculated resulting integer
                    // AND delete the now obselete instruction
                    llvm::ReplaceInstWithValue(B.getInstList(), I, llvm::ConstantInt::get(llvm::Type::getInt32Ty(F.getContext()), res));
                    changes = true;
                }
          } else if (auto *op = llvm::dyn_cast<llvm::CmpInst>(I)) {    // Check if the Instruction is a Comparison
                  llvm::Value *lhs = op->getOperand(0);
                  llvm::Value *rhs = op->getOperand(1);
                  if (llvm::isa<llvm::ConstantInt>(lhs) & llvm::isa<llvm::ConstantInt>(rhs)) {  // Check if both the left and right operands are constants
                    // Store the result of the comparison in cmpRes ('true' or 'false')
                    bool cmpRes;
                    switch(op->getPredicate()) {
                        case llvm::CmpInst::ICMP_EQ:            // '=='
                            cmpRes = (getInt(lhs) == getInt(rhs));
                            break;
                        case llvm::CmpInst::ICMP_SLT:           // '<'
                            cmpRes = (getInt(lhs) < getInt(rhs));
                            break;
                        case llvm::CmpInst::ICMP_SLE:           // '<='
                            cmpRes = (getInt(lhs) <= getInt(rhs));
                            break;
                        case llvm::CmpInst::ICMP_SGT:           // '>'
                            cmpRes = (getInt(lhs) > getInt(rhs));
                            break;
                        case llvm::CmpInst::ICMP_SGE:           // '>='
                            cmpRes = (getInt(lhs) >= getInt(rhs));
                            break;
                        case llvm::CmpInst::ICMP_NE:            // '!='
                            cmpRes = (getInt(lhs) != getInt(rhs));
                            break;
                        default:
                            cmpRes = false;
                            llvm::errs() << "unknown operator\n";
                            break;
                    }
                    // Replace the Instruction with 'true' or 'false' depending on the result of the comparison.
                    llvm::ReplaceInstWithValue(B.getInstList(), I, llvm::ConstantInt::get(llvm::Type::getInt1Ty(F.getContext()), cmpRes));
                    changes = true;
                }
          }
        }
      }
      // If constant folding/propagation has been done, keep running the pass on the function.
      if (changes) {
          runOnFunction(F);
      } else { // Otherwise, print out the optimized IR
          llvm::errs() << "Optimized Function:\n";
          F.print(llvm::errs());
          llvm::errs() << "--------------------\n";
      }
      return false;  // returning false means the overall CFG has not changed
    }
  };
}

// FoldPropPass naming
char FoldPropPass::ID = 0;
static llvm::RegisterPass<FoldPropPass> X("foldproppass", "Contant Folding Constant Propagation Pass",
			false,  /* looks at CFG, true changed CFG */
			false); /* analysis pass, true means analysis needs to run again */

// Enable the pass
static void registerFoldPropPass(const llvm::PassManagerBuilder &,
                         llvm::legacy::PassManagerBase &PM) {
  PM.add(new FoldPropPass());
}
static llvm::RegisterStandardPasses
  RegisterMyPass(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                 registerFoldPropPass);
