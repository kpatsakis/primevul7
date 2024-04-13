void WasmBinaryBuilder::visitReturn(Return* curr) {
  BYN_TRACE("zz node: Return\n");
  requireFunctionContext("return");
  Type type = currFunction->getResults();
  if (type.isConcrete()) {
    curr->value = popTypedExpression(type);
  }
  curr->finalize();
}