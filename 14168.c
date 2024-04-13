bool WasmBinaryBuilder::maybeVisitArrayNew(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::ArrayNew || code == BinaryConsts::ArrayNewDefault) {
    auto heapType = getIndexedHeapType();
    auto* size = popNonVoidExpression();
    Expression* init = nullptr;
    if (code == BinaryConsts::ArrayNew) {
      init = popNonVoidExpression();
    }
    out = Builder(wasm).makeArrayNew(heapType, size, init);
    return true;
  } else if (code == BinaryConsts::ArrayNewWithRtt ||
             code == BinaryConsts::ArrayNewDefaultWithRtt) {
    auto heapType = getIndexedHeapType();
    auto* rtt = popNonVoidExpression();
    validateHeapTypeUsingChild(rtt, heapType);
    auto* size = popNonVoidExpression();
    Expression* init = nullptr;
    if (code == BinaryConsts::ArrayNewWithRtt) {
      init = popNonVoidExpression();
    }
    out = Builder(wasm).makeArrayNew(rtt, size, init);
    return true;
  }
  return false;
}