void WasmBinaryBuilder::readMemoryAccess(Address& alignment, Address& offset) {
  auto rawAlignment = getU32LEB();
  if (rawAlignment > 4) {
    throwError("Alignment must be of a reasonable size");
  }
  alignment = Bits::pow2(rawAlignment);
  offset = getUPtrLEB();
}