void WasmBinaryWriter::prepare() {
  // Collect function types and their frequencies. Collect information in each
  // function in parallel, then merge.
  ModuleUtils::collectHeapTypes(*wasm, types, typeIndices);
  importInfo = wasm::make_unique<ImportInfo>(*wasm);
}