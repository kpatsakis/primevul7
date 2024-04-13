Name WasmBinaryBuilder::getTagName(Index index) {
  if (index >= wasm.tags.size()) {
    throwError("invalid tag index");
  }
  return wasm.tags[index]->name;
}