bool WasmBinaryBuilder::maybeVisitRefCast(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::RefCast) {
    auto* rtt = popNonVoidExpression();
    auto* ref = popNonVoidExpression();
    out = Builder(wasm).makeRefCast(ref, rtt);
    return true;
  } else if (code == BinaryConsts::RefCastStatic) {
    auto intendedType = getIndexedHeapType();
    auto* ref = popNonVoidExpression();
    out = Builder(wasm).makeRefCast(ref, intendedType);
    return true;
  }
  return false;
}