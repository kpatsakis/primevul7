bool WasmBinaryBuilder::maybeVisitRttSub(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::RttSub && code != BinaryConsts::RttFreshSub) {
    return false;
  }
  auto targetHeapType = getIndexedHeapType();
  auto* parent = popNonVoidExpression();
  if (code == BinaryConsts::RttSub) {
    out = Builder(wasm).makeRttSub(targetHeapType, parent);
  } else {
    out = Builder(wasm).makeRttFreshSub(targetHeapType, parent);
  }
  return true;
}