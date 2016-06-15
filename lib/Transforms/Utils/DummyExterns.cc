#include "llvm/Pass.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

namespace
{
  class DummyExterns : public ModulePass
  {
  public:
    static char ID;
    DummyExterns() : ModulePass(ID) {}

    bool runOnModule (Module &M)
      {
        bool change = false;

        Constant *stub = M.getOrInsertFunction("__seahorn_extern_stub", Type::getVoidTy (getGlobalContext ()), Type::getInt8Ty(getGlobalContext ())->getPointerTo (), NULL);
        assert (stub);

        for (auto &g : M.globals ()) {
          if (!g.hasInitializer ()) {
            g.setInitializer (Constant::getNullValue (g.getType ()));
            //g.dump ();
          }
        }
        for (auto &f : M.functions ()) {
          if (!f.empty ()) continue;
          if (f.getIntrinsicID () != Intrinsic::not_intrinsic) continue;
          if (f.getName () == "__seahorn_extern_stub" ||
              f.getName () == "exit" ||
              f.getName () == "__assert_fail" ||
              f.getName () == "printf") continue;

          //errs () << f.getName () << "\n";
          //f.dump ();
          BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "entry", &f);

          IRBuilder<> builder(bb);
          Constant *str = ConstantDataArray::getString (getGlobalContext (), f.getName ());
          Value *g = new GlobalVariable(M,
                           str->getType (),
                           true,
                           GlobalValue::PrivateLinkage,
                           str);

          builder.CreateCall(stub, makeArrayRef(g));
          Type *rettype = f.getReturnType ();
          if (rettype->isVoidTy ())
            builder.CreateRetVoid ();
          else
            builder.CreateRet (Constant::getNullValue (rettype));
        }

        return change;
      }
  };

  char DummyExterns::ID = 0;
}

namespace seahorn
{
  Pass *createDummyExternsPass () {return new DummyExterns ();}
}

static llvm::RegisterPass<DummyExterns>
X ("dummy-externs", "Add dummy bodies for external function calls");
