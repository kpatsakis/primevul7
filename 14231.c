bool WasmBinaryBuilder::maybeVisitArrayInit(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::ArrayInitStatic) {
    auto heapType = getIndexedHeapType();
    auto size = getU32LEB();
    std::vector<Expression*> values(size);
    for (size_t i = 0; i < size; i++) {
      values[size - i - 1] = popNonVoidExpression();
    }
    out = Builder(wasm).makeArrayInit(heapType, values);
    return true;
  } else if (code == BinaryConsts::ArrayInit) {
    auto heapType = getIndexedHeapType();
    auto size = getU32LEB();
    auto* rtt = popNonVoidExpression();
    validateHeapTypeUsingChild(rtt, heapType);
    std::vector<Expression*> values(size);
    for (size_t i = 0; i < size; i++) {
      values[size - i - 1] = popNonVoidExpression();
    }
    out = Builder(wasm).makeArrayInit(rtt, values);
    return true;
  }
  return false;
}