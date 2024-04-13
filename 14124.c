Name WasmBinaryBuilder::getInlineString() {
  BYN_TRACE("<==\n");
  auto len = getU32LEB();

  auto data = getByteView(len);

  std::string str(data.first, data.second);
  if (str.find('\0') != std::string::npos) {
    throwError(
      "inline string contains NULL (0). that is technically valid in wasm, "
      "but you shouldn't do it, and it's not supported in binaryen");
  }
  BYN_TRACE("getInlineString: " << str << " ==>\n");
  return Name(str);
}