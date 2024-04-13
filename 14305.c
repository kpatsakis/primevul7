uint64_t WasmBinaryBuilder::getUPtrLEB() {
  return wasm.memory.is64() ? getU64LEB() : getU32LEB();
}