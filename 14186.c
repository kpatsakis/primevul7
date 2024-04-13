Type WasmBinaryBuilder::getType(int initial) {
  // Single value types are negative; signature indices are non-negative
  if (initial >= 0) {
    // TODO: Handle block input types properly.
    return getSignatureByTypeIndex(initial).results;
  }
  Type type;
  if (getBasicType(initial, type)) {
    return type;
  }
  switch (initial) {
    // None only used for block signatures. TODO: Separate out?
    case BinaryConsts::EncodedType::Empty:
      return Type::none;
    case BinaryConsts::EncodedType::nullable:
      return Type(getHeapType(), Nullable);
    case BinaryConsts::EncodedType::nonnullable:
      return Type(getHeapType(), NonNullable);
    case BinaryConsts::EncodedType::rtt_n: {
      auto depth = getU32LEB();
      auto heapType = getIndexedHeapType();
      return Type(Rtt(depth, heapType));
    }
    case BinaryConsts::EncodedType::rtt: {
      return Type(Rtt(getIndexedHeapType()));
    }
    default:
      throwError("invalid wasm type: " + std::to_string(initial));
  }
  WASM_UNREACHABLE("unexpected type");
}