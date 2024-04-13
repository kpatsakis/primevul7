bool WasmBinaryBuilder::maybeVisitArraySet(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::ArraySet) {
    return false;
  }
  auto heapType = getIndexedHeapType();
  auto* value = popNonVoidExpression();
  auto* index = popNonVoidExpression();
  auto* ref = popNonVoidExpression();
  validateHeapTypeUsingChild(ref, heapType);
  out = Builder(wasm).makeArraySet(ref, index, value);
  return true;
}