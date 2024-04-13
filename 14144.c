void WasmBinaryBuilder::visitRefAs(RefAs* curr, uint8_t code) {
  BYN_TRACE("zz node: RefAs\n");
  switch (code) {
    case BinaryConsts::RefAsNonNull:
      curr->op = RefAsNonNull;
      break;
    case BinaryConsts::RefAsFunc:
      curr->op = RefAsFunc;
      break;
    case BinaryConsts::RefAsData:
      curr->op = RefAsData;
      break;
    case BinaryConsts::RefAsI31:
      curr->op = RefAsI31;
      break;
    default:
      WASM_UNREACHABLE("invalid code for ref.as_*");
  }
  curr->value = popNonVoidExpression();
  if (!curr->value->type.isRef() && curr->value->type != Type::unreachable) {
    throwError("bad input type for ref.as: " + curr->value->type.toString());
  }
  curr->finalize();
}