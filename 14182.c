void WasmBinaryBuilder::readTags() {
  BYN_TRACE("== readTags\n");
  size_t numTags = getU32LEB();
  BYN_TRACE("num: " << numTags << std::endl);
  for (size_t i = 0; i < numTags; i++) {
    BYN_TRACE("read one\n");
    getInt8(); // Reserved 'attribute' field
    auto typeIndex = getU32LEB();
    wasm.addTag(Builder::makeTag("tag$" + std::to_string(i),
                                 getSignatureByTypeIndex(typeIndex)));
  }
}