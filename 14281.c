void WasmBinaryBuilder::visitRefIs(RefIs* curr, uint8_t code) {
  BYN_TRACE("zz node: RefIs\n");
  switch (code) {
    case BinaryConsts::RefIsNull:
      curr->op = RefIsNull;
      break;
    case BinaryConsts::RefIsFunc:
      curr->op = RefIsFunc;
      break;
    case BinaryConsts::RefIsData:
      curr->op = RefIsData;
      break;
    case BinaryConsts::RefIsI31:
      curr->op = RefIsI31;
      break;
    default:
      WASM_UNREACHABLE("invalid code for ref.is_*");
  }
  curr->value = popNonVoidExpression();
  curr->finalize();
}