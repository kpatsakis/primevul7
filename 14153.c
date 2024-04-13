uint32_t WasmBinaryWriter::getTypeIndex(HeapType type) const {
  auto it = typeIndices.find(type);
#ifndef NDEBUG
  if (it == typeIndices.end()) {
    std::cout << "Missing type: " << type << '\n';
    assert(0);
  }
#endif
  return it->second;
}