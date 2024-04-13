bool WasmBinaryBuilder::maybeVisitStructNew(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::StructNew ||
      code == BinaryConsts::StructNewDefault) {
    auto heapType = getIndexedHeapType();
    std::vector<Expression*> operands;
    if (code == BinaryConsts::StructNew) {
      auto numOperands = heapType.getStruct().fields.size();
      operands.resize(numOperands);
      for (Index i = 0; i < numOperands; i++) {
        operands[numOperands - i - 1] = popNonVoidExpression();
      }
    }
    out = Builder(wasm).makeStructNew(heapType, operands);
    return true;
  } else if (code == BinaryConsts::StructNewWithRtt ||
             code == BinaryConsts::StructNewDefaultWithRtt) {
    auto heapType = getIndexedHeapType();
    auto* rtt = popNonVoidExpression();
    validateHeapTypeUsingChild(rtt, heapType);
    std::vector<Expression*> operands;
    if (code == BinaryConsts::StructNewWithRtt) {
      auto numOperands = heapType.getStruct().fields.size();
      operands.resize(numOperands);
      for (Index i = 0; i < numOperands; i++) {
        operands[numOperands - i - 1] = popNonVoidExpression();
      }
    }
    out = Builder(wasm).makeStructNew(rtt, operands);
    return true;
  }
  return false;
}