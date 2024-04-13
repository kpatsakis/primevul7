void WasmBinaryWriter::writeMemory() {
  if (!wasm->memory.exists || wasm->memory.imported()) {
    return;
  }
  BYN_TRACE("== writeMemory\n");
  auto start = startSection(BinaryConsts::Section::Memory);
  o << U32LEB(1); // Define 1 memory
  writeResizableLimits(wasm->memory.initial,
                       wasm->memory.max,
                       wasm->memory.hasMax(),
                       wasm->memory.shared,
                       wasm->memory.is64());
  finishSection(start);
}