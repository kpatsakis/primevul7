WasmBinaryBuilder::WasmBinaryBuilder(Module& wasm,
                                     FeatureSet features,
                                     const std::vector<char>& input)
  : wasm(wasm), allocator(wasm.allocator), input(input), sourceMap(nullptr),
    nextDebugLocation(0, {0, 0, 0}), debugLocation() {
  wasm.features = features;
}