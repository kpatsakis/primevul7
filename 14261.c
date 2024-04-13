bool WasmBinaryBuilder::maybeVisitRefTest(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::RefTest) {
    auto* rtt = popNonVoidExpression();
    auto* ref = popNonVoidExpression();
    out = Builder(wasm).makeRefTest(ref, rtt);
    return true;
  } else if (code == BinaryConsts::RefTestStatic) {
    auto intendedType = getIndexedHeapType();
    auto* ref = popNonVoidExpression();
    out = Builder(wasm).makeRefTest(ref, intendedType);
    return true;
  }
  return false;
}