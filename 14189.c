bool WasmBinaryBuilder::maybeVisitStructSet(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::StructSet) {
    return false;
  }
  auto* curr = allocator.alloc<StructSet>();
  auto heapType = getIndexedHeapType();
  curr->index = getU32LEB();
  curr->value = popNonVoidExpression();
  curr->ref = popNonVoidExpression();
  validateHeapTypeUsingChild(curr->ref, heapType);
  curr->finalize();
  out = curr;
  return true;
}