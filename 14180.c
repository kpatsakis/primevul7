void WasmBinaryBuilder::visitMemoryGrow(MemoryGrow* curr) {
  BYN_TRACE("zz node: MemoryGrow\n");
  curr->delta = popNonVoidExpression();
  auto reserved = getU32LEB();
  if (reserved != 0) {
    throwError("Invalid reserved field on memory.grow");
  }
  curr->finalize();
}