bool WasmBinaryBuilder::maybeVisitArrayGet(Expression*& out, uint32_t code) {
  bool signed_ = false;
  switch (code) {
    case BinaryConsts::ArrayGet:
    case BinaryConsts::ArrayGetU:
      break;
    case BinaryConsts::ArrayGetS:
      signed_ = true;
      break;
    default:
      return false;
  }
  auto heapType = getIndexedHeapType();
  auto* index = popNonVoidExpression();
  auto* ref = popNonVoidExpression();
  validateHeapTypeUsingChild(ref, heapType);
  out = Builder(wasm).makeArrayGet(ref, index, signed_);
  return true;
}