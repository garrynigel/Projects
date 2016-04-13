#include "llvm/Pass.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfo.h"
#include <string>
#include <algorithm>
#include <vector>

using namespace llvm;
using namespace std;

namespace {
  struct ArgumentCheck : public ModulePass{
    static char ID;
    struct Func{
    int calls;
    Function::ArgumentListType *argList;
    };
    ArgumentCheck() : ModulePass(ID) {}
    std::map<std::string,Func> defFuncMap;
    bool runOnModule(Module &M) override {
    Module::FunctionListType &funcList = M.getFunctionList();
    for(Module::FunctionListType::iterator loopstart = funcList.begin(),loopend = funcList.end();loopstart != loopend;++loopstart){
         defFuncMap[loopstart->getName()].argList = &(loopstart->getArgumentList());
         if(loopstart->getName() == "main")
             defFuncMap[loopstart->getName()].calls = 0;
    }

    for(Module::iterator loopstart = M.begin(),loopend = M.end();loopstart != loopend;++loopstart){

     runOnFunction(*loopstart,M);
    }

    printStatistics(funcList);

    return false;
    }

/*
     Run on each function in the Module
*/

    bool runOnFunction(Function &F, Module &M){

    for(Function::iterator loopstart = F.begin(),loopend = F.end();loopstart != loopend;++loopstart){

    runOnBasicBlock(*loopstart,M);

    }

    return false;
    }

/*

    Do a check on each Basic Block

*/
    bool runOnBasicBlock(BasicBlock &BB,Module &M)
    {


    std::vector<std::string> vecTypes = {"int", "float","double","char"};
    unsigned lineNum;

    for(BasicBlock::iterator BI = BB.begin(),BE = BB.end();BI != BE; ++BI)
    {
    if(isa<CallInst>(*BI))
    {
       CallInst *Instruct = dyn_cast<CallInst>(BI);
       Value *val = Instruct->getCalledValue();

       if(MDNode *N = BI->getMetadata("dbg")){

          DILocation Loc(N);
          lineNum = Loc.getLineNumber();
        }
       if(defFuncMap.find(val->stripPointerCasts()->getName()) != defFuncMap.end())
       {

        Function::ArgumentListType *CallFuncArgs = defFuncMap[val->stripPointerCasts()->getName()].argList;
        (defFuncMap[val->stripPointerCasts()->getName()].calls)++;

        if(Instruct->getNumArgOperands() == CallFuncArgs->size())
        {
           int i = 0;
           for(Function::ArgumentListType::iterator loopstart = CallFuncArgs->begin(),loopend = CallFuncArgs->end();loopstart != loopend;++loopstart ){

             Value *Arg = Instruct->getArgOperand(i);
             if(loopstart->getType() == Arg->getType())
             {
                i++;
                continue;
             }
             else if( (std::find(vecTypes.begin(),vecTypes.end(),returnType(*(Arg->getType()))) != vecTypes.end()) && ((std::find(vecTypes.begin(),vecTypes.end(),returnType(*(loopstart->getType()))))) != vecTypes.end())
             {
              errs()<<"Function "<<val->stripPointerCasts()->getName()<<" call on line "<<lineNum<<" : Warning :Argument type mismatch.Expected "<<returnType(*(loopstart->getType()))<<" but argument is of type "<<returnType(*(Arg->getType()))<<" \n";
             }
             else{
             errs()<<"Function "<<val->stripPointerCasts()->getName()<<" call on line "<<lineNum<<" : Error : Argument type mismatch.Expected "<<returnType(*(loopstart->getType()))<<" but argument is of type "<<returnType(*(Arg->getType()))<<" \n";
             }

             i++;

           }
        }
        else{

        if(val->stripPointerCasts()->getName() == "printf")
           continue;

         errs()<<"Function "<<val->stripPointerCasts()->getName()<<" call on line "<<lineNum<<" : Error : Expected "<<CallFuncArgs->size()<<" but "<<Instruct->getNumArgOperands()<<" are/is present \n";


        }


       }
       else{
         if (val->stripPointerCasts()->getName() == "")
         continue;

          errs()<<"Function "<<val->stripPointerCasts()->getName()<<" call on line "<<lineNum<<" : Error : Not Defined \n";
       }



    }

    }

    return false;
    }

    string returnType(Type &argType)
    {
      string returnType;
      if(argType.isIntegerTy(8)){
      returnType = "char";
      }
      else if (argType.isIntegerTy()){
      returnType = "int";
      }
      else if (argType.isStructTy()){
      returnType = "struct";
      }
      else if(argType.isArrayTy()){
      returnType = "array";
      }
      else if(argType.isPointerTy()){
      returnType = "pointer";
      }
      else if(argType.isFloatTy()){
      returnType = "float";
      }
      else if(argType.isDoubleTy()){
      returnType = "double";
      }
      else if(argType.isFunctionTy()){
      returnType = "function";
      }
      else{
      returnType = "char";
      }


      return returnType;
    }

  bool printStatistics(Module::FunctionListType &funcList){


    errs()<<"List of function calls \n";
    for(Module::FunctionListType::iterator loopstart = funcList.begin(),loopend = funcList.end();loopstart != loopend;++loopstart){
         defFuncMap[loopstart->getName()].argList = &(loopstart->getArgumentList());
         if(loopstart->getName() != "llvm.dbg.declare" && loopstart->getName() != "printf")
         {
             errs()<<loopstart->getName();
             Function::ArgumentListType *CallFuncArgs = defFuncMap[loopstart->getName()].argList;
             if (CallFuncArgs->size() != 0){
             errs()<<"(";
             int num = 0;
             for(Function::ArgumentListType::iterator loopstart = CallFuncArgs->begin(),loopend = CallFuncArgs->end();loopstart != loopend;++loopstart ){

                 if (num != 0)
                   errs()<<",";

                 errs()<<returnType(*(loopstart->getType()));

                 num++;

             }

             errs()<<")";
             }
             errs()<<" : "<<defFuncMap[loopstart->getName()].calls<<"\n";

         }
    }

    return false;
  }

  };
}

char ArgumentCheck::ID = 0;
static RegisterPass<ArgumentCheck> X("ArgumentCheck", "Function Argument Check Pass", false, false);
