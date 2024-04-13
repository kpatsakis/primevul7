  Name process(Name name) {
    return deduplicate(WasmBinaryBuilder::escape(name));
  }