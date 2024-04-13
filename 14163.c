void WasmBinaryBuilder::readDylink0(size_t payloadLen) {
  BYN_TRACE("== readDylink0\n");
  auto sectionPos = pos;
  uint32_t lastType = 0;

  wasm.dylinkSection = make_unique<DylinkSection>();
  while (pos < sectionPos + payloadLen) {
    auto oldPos = pos;
    auto dylinkType = getU32LEB();
    if (lastType && dylinkType <= lastType) {
      std::cerr << "warning: out-of-order dylink.0 subsection: " << dylinkType
                << std::endl;
    }
    lastType = dylinkType;
    auto subsectionSize = getU32LEB();
    auto subsectionPos = pos;
    if (dylinkType == BinaryConsts::UserSections::Subsection::DylinkMemInfo) {
      wasm.dylinkSection->memorySize = getU32LEB();
      wasm.dylinkSection->memoryAlignment = getU32LEB();
      wasm.dylinkSection->tableSize = getU32LEB();
      wasm.dylinkSection->tableAlignment = getU32LEB();
    } else if (dylinkType ==
               BinaryConsts::UserSections::Subsection::DylinkNeeded) {
      size_t numNeededDynlibs = getU32LEB();
      for (size_t i = 0; i < numNeededDynlibs; ++i) {
        wasm.dylinkSection->neededDynlibs.push_back(getInlineString());
      }
    } else {
      // Unknown subsection.  Stop parsing now and store the rest of
      // the section verbatim.
      pos = oldPos;
      size_t remaining = (sectionPos + payloadLen) - pos;
      auto tail = getByteView(remaining);
      wasm.dylinkSection->tail = {tail.first, tail.second};
      break;
    }
    if (pos != subsectionPos + subsectionSize) {
      throwError("bad dylink.0 subsection position change");
    }
  }
}