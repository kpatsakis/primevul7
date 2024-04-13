bool WasmBinaryBuilder::maybeVisitI31New(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::I31New) {
    return false;
  }
  auto* curr = allocator.alloc<I31New>();
  curr->value = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}