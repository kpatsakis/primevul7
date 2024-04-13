void WasmBinaryWriter::writeDataSegments() {
  if (wasm->memory.segments.size() == 0) {
    return;
  }
  if (wasm->memory.segments.size() > WebLimitations::MaxDataSegments) {
    std::cerr << "Some VMs may not accept this binary because it has a large "
              << "number of data segments. Run the limit-segments pass to "
              << "merge segments.\n";
  }
  auto start = startSection(BinaryConsts::Section::Data);
  o << U32LEB(wasm->memory.segments.size());
  for (auto& segment : wasm->memory.segments) {
    uint32_t flags = 0;
    if (segment.isPassive) {
      flags |= BinaryConsts::IsPassive;
    }
    o << U32LEB(flags);
    if (!segment.isPassive) {
      writeExpression(segment.offset);
      o << int8_t(BinaryConsts::End);
    }
    writeInlineBuffer(segment.data.data(), segment.data.size());
  }
  finishSection(start);
}