void WasmBinaryBuilder::throwError(std::string text) {
  throw ParseException(text, 0, pos);
}