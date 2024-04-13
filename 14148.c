bool WasmBinaryBuilder::maybeVisitArrayLen(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::ArrayLen) {
    return false;
  }
  auto heapType = getIndexedHeapType();
  auto* ref = popNonVoidExpression();
  validateHeapTypeUsingChild(ref, heapType);
  out = Builder(wasm).makeArrayLen(ref);
  return true;
}