void WasmBinaryBuilder::readTableDeclarations() {
  BYN_TRACE("== readTableDeclarations\n");
  auto numTables = getU32LEB();

  for (size_t i = 0; i < numTables; i++) {
    auto elemType = getType();
    if (!elemType.isRef()) {
      throwError("Table type must be a reference type");
    }
    auto table = Builder::makeTable(Name::fromInt(i), elemType);
    bool is_shared;
    Type indexType;
    getResizableLimits(
      table->initial, table->max, is_shared, indexType, Table::kUnlimitedSize);
    if (is_shared) {
      throwError("Tables may not be shared");
    }
    if (indexType == Type::i64) {
      throwError("Tables may not be 64-bit");
    }

    tables.push_back(std::move(table));
  }
}