bool WasmBinaryBuilder::maybeVisitI31Get(Expression*& out, uint32_t code) {
  I31Get* curr;
  switch (code) {
    case BinaryConsts::I31GetS:
      curr = allocator.alloc<I31Get>();
      curr->signed_ = true;
      break;
    case BinaryConsts::I31GetU:
      curr = allocator.alloc<I31Get>();
      curr->signed_ = false;
      break;
    default:
      return false;
  }
  curr->i31 = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}