void WasmBinaryBuilder::readDataSegments() {
  BYN_TRACE("== readDataSegments\n");
  auto num = getU32LEB();
  for (size_t i = 0; i < num; i++) {
    Memory::Segment curr;
    uint32_t flags = getU32LEB();
    if (flags > 2) {
      throwError("bad segment flags, must be 0, 1, or 2, not " +
                 std::to_string(flags));
    }
    curr.isPassive = flags & BinaryConsts::IsPassive;
    if (flags & BinaryConsts::HasIndex) {
      auto memIndex = getU32LEB();
      if (memIndex != 0) {
        throwError("nonzero memory index");
      }
    }
    if (!curr.isPassive) {
      curr.offset = readExpression();
    }
    auto size = getU32LEB();
    auto data = getByteView(size);
    curr.data = {data.first, data.second};
    wasm.memory.segments.push_back(std::move(curr));
  }
}