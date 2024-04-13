bool WasmBinaryBuilder::maybeVisitArrayCopy(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::ArrayCopy) {
    return false;
  }
  auto destHeapType = getIndexedHeapType();
  auto srcHeapType = getIndexedHeapType();
  auto* length = popNonVoidExpression();
  auto* srcIndex = popNonVoidExpression();
  auto* srcRef = popNonVoidExpression();
  auto* destIndex = popNonVoidExpression();
  auto* destRef = popNonVoidExpression();
  validateHeapTypeUsingChild(destRef, destHeapType);
  validateHeapTypeUsingChild(srcRef, srcHeapType);
  out =
    Builder(wasm).makeArrayCopy(destRef, destIndex, srcRef, srcIndex, length);
  return true;
}