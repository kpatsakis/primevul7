void WasmBinaryWriter::writeType(Type type) {
  if (type.isRef() && !type.isBasic()) {
    if (type.isNullable()) {
      o << S32LEB(BinaryConsts::EncodedType::nullable);
    } else {
      o << S32LEB(BinaryConsts::EncodedType::nonnullable);
    }
    writeHeapType(type.getHeapType());
    return;
  }
  if (type.isRtt()) {
    auto rtt = type.getRtt();
    if (rtt.hasDepth()) {
      o << S32LEB(BinaryConsts::EncodedType::rtt_n);
      o << U32LEB(rtt.depth);
    } else {
      o << S32LEB(BinaryConsts::EncodedType::rtt);
    }
    writeIndexedHeapType(rtt.heapType);
    return;
  }
  int ret = 0;
  TODO_SINGLE_COMPOUND(type);
  switch (type.getBasic()) {
    // None only used for block signatures. TODO: Separate out?
    case Type::none:
      ret = BinaryConsts::EncodedType::Empty;
      break;
    case Type::i32:
      ret = BinaryConsts::EncodedType::i32;
      break;
    case Type::i64:
      ret = BinaryConsts::EncodedType::i64;
      break;
    case Type::f32:
      ret = BinaryConsts::EncodedType::f32;
      break;
    case Type::f64:
      ret = BinaryConsts::EncodedType::f64;
      break;
    case Type::v128:
      ret = BinaryConsts::EncodedType::v128;
      break;
    case Type::funcref:
      ret = BinaryConsts::EncodedType::funcref;
      break;
    case Type::externref:
      ret = BinaryConsts::EncodedType::externref;
      break;
    case Type::anyref:
      ret = BinaryConsts::EncodedType::anyref;
      break;
    case Type::eqref:
      ret = BinaryConsts::EncodedType::eqref;
      break;
    case Type::i31ref:
      ret = BinaryConsts::EncodedType::i31ref;
      break;
    case Type::dataref:
      ret = BinaryConsts::EncodedType::dataref;
      break;
    default:
      WASM_UNREACHABLE("unexpected type");
  }
  o << S32LEB(ret);
}