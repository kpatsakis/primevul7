void WasmBinaryBuilder::readDataCount() {
  BYN_TRACE("== readDataCount\n");
  hasDataCount = true;
  dataCount = getU32LEB();
}