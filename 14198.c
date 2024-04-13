bool WasmBinaryBuilder::maybeVisitStructGet(Expression*& out, uint32_t code) {
  StructGet* curr;
  switch (code) {
    case BinaryConsts::StructGet:
      curr = allocator.alloc<StructGet>();
      break;
    case BinaryConsts::StructGetS:
      curr = allocator.alloc<StructGet>();
      curr->signed_ = true;
      break;
    case BinaryConsts::StructGetU:
      curr = allocator.alloc<StructGet>();
      curr->signed_ = false;
      break;
    default:
      return false;
  }
  auto heapType = getIndexedHeapType();
  curr->index = getU32LEB();
  curr->ref = popNonVoidExpression();
  validateHeapTypeUsingChild(curr->ref, heapType);
  curr->finalize();
  out = curr;
  return true;
}