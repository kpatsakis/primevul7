void WasmBinaryBuilder::readMemory() {
  BYN_TRACE("== readMemory\n");
  auto numMemories = getU32LEB();
  if (!numMemories) {
    return;
  }
  if (numMemories != 1) {
    throwError("Must be exactly 1 memory");
  }
  if (wasm.memory.exists) {
    throwError("Memory cannot be both imported and defined");
  }
  wasm.memory.exists = true;
  getResizableLimits(wasm.memory.initial,
                     wasm.memory.max,
                     wasm.memory.shared,
                     wasm.memory.indexType,
                     Memory::kUnlimitedSize);
}