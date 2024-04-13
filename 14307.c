void WasmBinaryBuilder::validateBinary() {
  if (hasDataCount && wasm.memory.segments.size() != dataCount) {
    throwError("Number of segments does not agree with DataCount section");
  }
}