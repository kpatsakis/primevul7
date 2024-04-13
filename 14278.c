bool WasmBinaryBuilder::getBasicType(int32_t code, Type& out) {
  switch (code) {
    case BinaryConsts::EncodedType::i32:
      out = Type::i32;
      return true;
    case BinaryConsts::EncodedType::i64:
      out = Type::i64;
      return true;
    case BinaryConsts::EncodedType::f32:
      out = Type::f32;
      return true;
    case BinaryConsts::EncodedType::f64:
      out = Type::f64;
      return true;
    case BinaryConsts::EncodedType::v128:
      out = Type::v128;
      return true;
    case BinaryConsts::EncodedType::funcref:
      out = Type::funcref;
      return true;
    case BinaryConsts::EncodedType::externref:
      out = Type::externref;
      return true;
    case BinaryConsts::EncodedType::anyref:
      out = Type::anyref;
      return true;
    case BinaryConsts::EncodedType::eqref:
      out = Type::eqref;
      return true;
    case BinaryConsts::EncodedType::i31ref:
      out = Type(HeapType::i31, NonNullable);
      return true;
    case BinaryConsts::EncodedType::dataref:
      out = Type(HeapType::data, NonNullable);
      return true;
    default:
      return false;
  }
}