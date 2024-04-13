void WasmBinaryWriter::writeHeapType(HeapType type) {
  if (type.isSignature() || type.isStruct() || type.isArray()) {
    o << S64LEB(getTypeIndex(type)); // TODO: Actually s33
    return;
  }
  int ret = 0;
  if (type.isBasic()) {
    switch (type.getBasic()) {
      case HeapType::func:
        ret = BinaryConsts::EncodedHeapType::func;
        break;
      case HeapType::ext:
        ret = BinaryConsts::EncodedHeapType::extern_;
        break;
      case HeapType::any:
        ret = BinaryConsts::EncodedHeapType::any;
        break;
      case HeapType::eq:
        ret = BinaryConsts::EncodedHeapType::eq;
        break;
      case HeapType::i31:
        ret = BinaryConsts::EncodedHeapType::i31;
        break;
      case HeapType::data:
        ret = BinaryConsts::EncodedHeapType::data;
        break;
    }
  } else {
    WASM_UNREACHABLE("TODO: compound GC types");
  }
  o << S64LEB(ret); // TODO: Actually s33
}