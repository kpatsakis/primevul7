void WasmBinaryBuilder::visitMemorySize(MemorySize* curr) {
  BYN_TRACE("zz node: MemorySize\n");
  auto reserved = getU32LEB();
  if (reserved != 0) {
    throwError("Invalid reserved field on memory.size");
  }
  curr->finalize();
}