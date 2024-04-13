bool WasmBinaryBuilder::maybeVisitRttCanon(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::RttCanon) {
    return false;
  }
  auto heapType = getIndexedHeapType();
  out = Builder(wasm).makeRttCanon(heapType);
  return true;
}