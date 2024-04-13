void WasmBinaryBuilder::readVars() {
  size_t numLocalTypes = getU32LEB();
  for (size_t t = 0; t < numLocalTypes; t++) {
    auto num = getU32LEB();
    auto type = getConcreteType();
    while (num > 0) {
      currFunction->vars.push_back(type);
      num--;
    }
  }
}