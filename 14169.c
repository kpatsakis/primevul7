void WasmBinaryWriter::writeUserSection(const UserSection& section) {
  auto start = startSection(BinaryConsts::User);
  writeInlineString(section.name.c_str());
  for (size_t i = 0; i < section.data.size(); i++) {
    o << uint8_t(section.data[i]);
  }
  finishSection(start);
}