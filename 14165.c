void WasmBinaryBuilder::readTypes() {
  BYN_TRACE("== readTypes\n");
  size_t numTypes = getU32LEB();
  BYN_TRACE("num: " << numTypes << std::endl);
  TypeBuilder builder(numTypes);

  auto makeType = [&](int32_t typeCode) {
    Type type;
    if (getBasicType(typeCode, type)) {
      return type;
    }

    switch (typeCode) {
      case BinaryConsts::EncodedType::nullable:
      case BinaryConsts::EncodedType::nonnullable: {
        auto nullability = typeCode == BinaryConsts::EncodedType::nullable
                             ? Nullable
                             : NonNullable;
        int64_t htCode = getS64LEB(); // TODO: Actually s33
        HeapType ht;
        if (getBasicHeapType(htCode, ht)) {
          return Type(ht, nullability);
        }
        if (size_t(htCode) >= numTypes) {
          throwError("invalid type index: " + std::to_string(htCode));
        }
        return builder.getTempRefType(builder[size_t(htCode)], nullability);
      }
      case BinaryConsts::EncodedType::rtt_n:
      case BinaryConsts::EncodedType::rtt: {
        auto depth = typeCode == BinaryConsts::EncodedType::rtt ? Rtt::NoDepth
                                                                : getU32LEB();
        auto htCode = getU32LEB();
        if (size_t(htCode) >= numTypes) {
          throwError("invalid type index: " + std::to_string(htCode));
        }
        return builder.getTempRttType(Rtt(depth, builder[htCode]));
      }
      default:
        throwError("unexpected type index: " + std::to_string(typeCode));
    }
    WASM_UNREACHABLE("unexpected type");
  };

  auto readType = [&]() { return makeType(getS32LEB()); };

  auto readSignatureDef = [&]() {
    std::vector<Type> params;
    std::vector<Type> results;
    size_t numParams = getU32LEB();
    BYN_TRACE("num params: " << numParams << std::endl);
    for (size_t j = 0; j < numParams; j++) {
      params.push_back(readType());
    }
    auto numResults = getU32LEB();
    BYN_TRACE("num results: " << numResults << std::endl);
    for (size_t j = 0; j < numResults; j++) {
      results.push_back(readType());
    }
    return Signature(builder.getTempTupleType(params),
                     builder.getTempTupleType(results));
  };

  auto readMutability = [&]() {
    switch (getU32LEB()) {
      case 0:
        return Immutable;
      case 1:
        return Mutable;
      default:
        throw ParseException("Expected 0 or 1 for mutability");
    }
  };

  auto readFieldDef = [&]() {
    // The value may be a general wasm type, or one of the types only possible
    // in a field.
    auto typeCode = getS32LEB();
    if (typeCode == BinaryConsts::EncodedType::i8) {
      auto mutable_ = readMutability();
      return Field(Field::i8, mutable_);
    }
    if (typeCode == BinaryConsts::EncodedType::i16) {
      auto mutable_ = readMutability();
      return Field(Field::i16, mutable_);
    }
    // It's a regular wasm value.
    auto type = makeType(typeCode);
    auto mutable_ = readMutability();
    return Field(type, mutable_);
  };

  auto readStructDef = [&]() {
    FieldList fields;
    size_t numFields = getU32LEB();
    BYN_TRACE("num fields: " << numFields << std::endl);
    for (size_t j = 0; j < numFields; j++) {
      fields.push_back(readFieldDef());
    }
    return Struct(std::move(fields));
  };

  for (size_t i = 0; i < numTypes; i++) {
    BYN_TRACE("read one\n");
    auto form = getS32LEB();
    if (form == BinaryConsts::EncodedType::Func ||
        form == BinaryConsts::EncodedType::FuncExtending) {
      builder[i] = readSignatureDef();
    } else if (form == BinaryConsts::EncodedType::Struct ||
               form == BinaryConsts::EncodedType::StructExtending) {
      builder[i] = readStructDef();
    } else if (form == BinaryConsts::EncodedType::Array ||
               form == BinaryConsts::EncodedType::ArrayExtending) {
      builder[i] = Array(readFieldDef());
    } else {
      throwError("bad type form " + std::to_string(form));
    }
    if (form == BinaryConsts::EncodedType::FuncExtending ||
        form == BinaryConsts::EncodedType::StructExtending ||
        form == BinaryConsts::EncodedType::ArrayExtending) {
      // TODO: Let the new nominal types coexist with equirecursive types
      // builder[i].setNominal();
      auto superIndex = getS64LEB(); // TODO: Actually s33
      if (superIndex >= 0) {
        if (size_t(superIndex) >= numTypes) {
          throwError("bad supertype index " + std::to_string(superIndex));
        }
        builder[i].subTypeOf(builder[superIndex]);
      } else {
        // Validate but otherwise ignore trivial supertypes.
        HeapType super;
        if (!getBasicHeapType(superIndex, super)) {
          throwError("Unrecognized supertype " + std::to_string(superIndex));
        }
        if (form == BinaryConsts::EncodedType::FuncExtending) {
          if (super != HeapType::func) {
            throwError(
              "The only allowed trivial supertype for functions is func");
          }
        } else {
          if (super != HeapType::data) {
            throwError("The only allowed trivial supertype for structs and "
                       "arrays is data");
          }
        }
      }
    }
  }

  types = builder.build();
}