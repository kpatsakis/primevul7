void WasmBinaryWriter::writeTypes() {
  if (types.size() == 0) {
    return;
  }
  BYN_TRACE("== writeTypes\n");
  auto start = startSection(BinaryConsts::Section::Type);
  o << U32LEB(types.size());
  for (Index i = 0; i < types.size(); ++i) {
    auto type = types[i];
    bool nominal = type.isNominal() || getTypeSystem() == TypeSystem::Nominal;
    BYN_TRACE("write " << type << std::endl);
    if (type.isSignature()) {
      o << S32LEB(nominal ? BinaryConsts::EncodedType::FuncExtending
                          : BinaryConsts::EncodedType::Func);
      auto sig = type.getSignature();
      for (auto& sigType : {sig.params, sig.results}) {
        o << U32LEB(sigType.size());
        for (const auto& type : sigType) {
          writeType(type);
        }
      }
    } else if (type.isStruct()) {
      o << S32LEB(nominal ? BinaryConsts::EncodedType::StructExtending
                          : BinaryConsts::EncodedType::Struct);
      auto fields = type.getStruct().fields;
      o << U32LEB(fields.size());
      for (const auto& field : fields) {
        writeField(field);
      }
    } else if (type.isArray()) {
      o << S32LEB(nominal ? BinaryConsts::EncodedType::ArrayExtending
                          : BinaryConsts::EncodedType::Array);
      writeField(type.getArray().element);
    } else {
      WASM_UNREACHABLE("TODO GC type writing");
    }
    if (nominal) {
      auto super = type.getSuperType();
      if (!super) {
        super = type.isFunction() ? HeapType::func : HeapType::data;
      }
      writeHeapType(*super);
    }
  }
  finishSection(start);
}