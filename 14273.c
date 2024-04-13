void WasmBinaryBuilder::readUserSection(size_t payloadLen) {
  BYN_TRACE("== readUserSection\n");
  auto oldPos = pos;
  Name sectionName = getInlineString();
  size_t read = pos - oldPos;
  if (read > payloadLen) {
    throwError("bad user section size");
  }
  payloadLen -= read;
  if (sectionName.equals(BinaryConsts::UserSections::Name)) {
    if (debugInfo) {
      readNames(payloadLen);
    } else {
      pos += payloadLen;
    }
  } else if (sectionName.equals(BinaryConsts::UserSections::TargetFeatures)) {
    readFeatures(payloadLen);
  } else if (sectionName.equals(BinaryConsts::UserSections::Dylink)) {
    readDylink(payloadLen);
  } else if (sectionName.equals(BinaryConsts::UserSections::Dylink0)) {
    readDylink0(payloadLen);
  } else {
    // an unfamiliar custom section
    if (sectionName.equals(BinaryConsts::UserSections::Linking)) {
      std::cerr
        << "warning: linking section is present, so this is not a standard "
           "wasm file - binaryen cannot handle this properly!\n";
    }
    wasm.userSections.resize(wasm.userSections.size() + 1);
    auto& section = wasm.userSections.back();
    section.name = sectionName.str;
    auto data = getByteView(payloadLen);
    section.data = {data.first, data.second};
  }
}