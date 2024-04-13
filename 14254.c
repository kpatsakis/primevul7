bool WasmBinaryBuilder::maybeVisitTableSize(Expression*& out, uint32_t code) {
  if (code != BinaryConsts::TableSize) {
    return false;
  }
  Index tableIdx = getU32LEB();
  if (tableIdx >= tables.size()) {
    throwError("bad table index");
  }
  auto* curr = allocator.alloc<TableSize>();
  curr->finalize();
  // Defer setting the table name for later, when we know it.
  tableRefs[tableIdx].push_back(curr);
  out = curr;
  return true;
}