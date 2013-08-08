#ifndef _EMUL_LLVM_
#define _EMUL_LLVM_

#include <medusa/emulation.hpp>

#include <llvm/Support/TargetSelect.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/DataLayout.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Target/TargetMachine.h>

#if defined(_WIN32) || defined(WIN32)
# ifdef emul_llvm_EXPORTS
#  define EMUL_LLVM_EXPORT __declspec(dllexport)
# else
#  define EMUL_LLVM_EXPORT __declspec(dllimport)
# endif
#else
# define EMUL_LLVM_EXPORT
#endif

MEDUSA_NAMESPACE_USE

  extern "C" EMUL_LLVM_EXPORT Emulator* GetEmulator(CpuInformation const* pCpuInfo, CpuContext* pCpuCtxt, MemoryContext *pMemCtxt, VariableContext *pVarCtxt);

class LlvmEmulator : public medusa::Emulator
{
public:
  LlvmEmulator(CpuInformation const* pCpuInfo, CpuContext* pCpuCtxt, MemoryContext *pMemCtxt, VariableContext *pVarCtxt);
  virtual ~LlvmEmulator(void);

  virtual std::string GetName(void) const { return "llvm"; }

  virtual bool Execute(Expression const& rExpr);
  virtual bool Execute(Expression::List const& rExprList);

private:
  llvm::IRBuilder<>             m_Builder;
  static llvm::Module*          sm_pModule;
  static llvm::ExecutionEngine* sm_pExecutionEngine;
  static llvm::DataLayout*      sm_pDataLayout;

  class LlvmExpressionVisitor : public ExpressionVisitor
  {
  public:
    LlvmExpressionVisitor(HookAddressHashMap const& Hooks, CpuContext* pCpuCtxt, MemoryContext* pMemCtxt, VariableContext* pVarCtxt);
    virtual Expression* VisitBind(Expression::List const& rExprList);
    virtual Expression* VisitCondition(u32 Type, Expression const* pRefExpr, Expression const* pTestExpr);
    virtual Expression* VisitIfCondition(u32 Type, Expression const* pRefExpr, Expression const* pTestExpr, Expression const* pThenExpr);
    virtual Expression* VisitIfElseCondition(u32 Type, Expression const* pRefExpr, Expression const* pTestExpr, Expression const* pThenExpr, Expression const* pElseExpr);
    virtual Expression* VisitWhileCondition(u32 Type, Expression const* pRefExpr, Expression const* pTestExpr, Expression const* pBodyExpr);
    virtual Expression* VisitOperation(u32 Type, Expression const* pLeftExpr, Expression const* pRightExpr);
    virtual Expression* VisitConstant(u32 Type, u64 Value);
    virtual Expression* VisitIdentifier(u32 Id, CpuInformation const* pCpuInfo);
    virtual Expression* VisitMemory(u32 AccessSizeInBit, Expression const* pBaseExpr, Expression const* pOffsetExpr, bool Deref);
    virtual Expression* VisitVariable(u32 SizeInBit, std::string const& rName);

  protected:
    HookAddressHashMap const& m_rHooks;
    CpuContext*               m_pCpuCtxt;
    MemoryContext*            m_pMemCtxt;
    VariableContext*          m_pVarCtxt;
  };
};

#endif // !_EMUL_LLVM_