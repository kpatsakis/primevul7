void WasmBinaryBuilder::readElementSegments() {
  BYN_TRACE("== readElementSegments\n");
  auto numSegments = getU32LEB();
  if (numSegments >= Table::kMaxSize) {
    throwError("Too many segments");
  }
  for (size_t i = 0; i < numSegments; i++) {
    auto flags = getU32LEB();
    bool isPassive = (flags & BinaryConsts::IsPassive) != 0;
    bool hasTableIdx = !isPassive && ((flags & BinaryConsts::HasIndex) != 0);
    bool isDeclarative =
      isPassive && ((flags & BinaryConsts::IsDeclarative) != 0);
    bool usesExpressions = (flags & BinaryConsts::UsesExpressions) != 0;

    if (isDeclarative) {
      // Declared segments are needed in wasm text and binary, but not in
      // Binaryen IR; skip over the segment
      auto type = getU32LEB();
      WASM_UNUSED(type);
      auto num = getU32LEB();
      for (Index i = 0; i < num; i++) {
        getU32LEB();
      }
      continue;
    }

    auto segment = std::make_unique<ElementSegment>();
    segment->setName(Name::fromInt(i), false);

    if (!isPassive) {
      Index tableIdx = 0;
      if (hasTableIdx) {
        tableIdx = getU32LEB();
      }

      Table* table = nullptr;
      auto numTableImports = tableImports.size();
      if (tableIdx < numTableImports) {
        table = tableImports[tableIdx];
      } else if (tableIdx - numTableImports < tables.size()) {
        table = tables[tableIdx - numTableImports].get();
      }
      if (!table) {
        throwError("Table index out of range.");
      }

      segment->table = table->name;
      segment->offset = readExpression();
    }

    if (isPassive || hasTableIdx) {
      if (usesExpressions) {
        segment->type = getType();
        if (!segment->type.isFunction()) {
          throwError("Invalid type for a usesExpressions element segment");
        }
      } else {
        auto elemKind = getU32LEB();
        if (elemKind != 0x0) {
          throwError("Invalid kind (!= funcref(0)) since !usesExpressions.");
        }
      }
    }

    auto& segmentData = segment->data;
    auto size = getU32LEB();
    if (usesExpressions) {
      for (Index j = 0; j < size; j++) {
        segmentData.push_back(readExpression());
      }
    } else {
      for (Index j = 0; j < size; j++) {
        Index index = getU32LEB();
        auto sig = getTypeByFunctionIndex(index);
        // Use a placeholder name for now
        auto* refFunc = Builder(wasm).makeRefFunc(Name::fromInt(index), sig);
        functionRefs[index].push_back(refFunc);
        segmentData.push_back(refFunc);
      }
    }

    elementSegments.push_back(std::move(segment));
  }
}