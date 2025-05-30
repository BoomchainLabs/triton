#ifndef TRITON_DIALECT
#define TRITON_DIALECT

include "mlir/IR/OpBase.td"

def Triton_Dialect : Dialect {
  let name = "tt";

  let cppNamespace = "::mlir::triton";

  let summary = "The Triton IR in MLIR";

  let description = [{
    Triton Dialect.

    Dependent Dialects:
      * Arith:
        * addf, addi, andi, cmpf, cmpi, divf, fptosi, ...
      * Math:
        * exp, sin, cos, log, ...
      * StructuredControlFlow:
        * for, if, while, yield, condition
      * ControlFlow:
        * br, cond_br
  }];

  let dependentDialects = [
    "arith::ArithDialect",
    "math::MathDialect",
    "scf::SCFDialect",
    "cf::ControlFlowDialect",
    "ub::UBDialect"
  ];

  let extraClassDeclaration = [{
    void registerTypes();

    static TritonDialect *getLoaded(MLIRContext *ctx) {
      return ctx->getLoadedDialect<TritonDialect>();
    }
    static TritonDialect *getLoaded(Operation *op) {
      return getLoaded(op->getContext());
    }
  }];

  let discardableAttrs = (ins
     "::mlir::IntegerAttr":$num_stages,
     "::mlir::IntegerAttr":$latency,
     "::mlir::IntegerAttr":$self_latency
  );

  let hasConstantMaterializer = 1;
  let useDefaultTypePrinterParser = 1;
  let usePropertiesForAttributes = 1;
}

include "triton/Dialect/Triton/IR/TritonTypes.td"


#endif // TRITON_DIALECT
