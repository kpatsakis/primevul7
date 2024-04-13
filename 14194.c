void WasmBinaryWriter::writeFeaturesSection() {
  if (!wasm->hasFeaturesSection || wasm->features.isMVP()) {
    return;
  }

  // TODO(tlively): unify feature names with rest of toolchain and use
  // FeatureSet::toString()
  auto toString = [](FeatureSet::Feature f) {
    switch (f) {
      case FeatureSet::Atomics:
        return BinaryConsts::UserSections::AtomicsFeature;
      case FeatureSet::MutableGlobals:
        return BinaryConsts::UserSections::MutableGlobalsFeature;
      case FeatureSet::TruncSat:
        return BinaryConsts::UserSections::TruncSatFeature;
      case FeatureSet::SIMD:
        return BinaryConsts::UserSections::SIMD128Feature;
      case FeatureSet::BulkMemory:
        return BinaryConsts::UserSections::BulkMemoryFeature;
      case FeatureSet::SignExt:
        return BinaryConsts::UserSections::SignExtFeature;
      case FeatureSet::ExceptionHandling:
        return BinaryConsts::UserSections::ExceptionHandlingFeature;
      case FeatureSet::TailCall:
        return BinaryConsts::UserSections::TailCallFeature;
      case FeatureSet::ReferenceTypes:
        return BinaryConsts::UserSections::ReferenceTypesFeature;
      case FeatureSet::Multivalue:
        return BinaryConsts::UserSections::MultivalueFeature;
      case FeatureSet::GC:
        return BinaryConsts::UserSections::GCFeature;
      case FeatureSet::Memory64:
        return BinaryConsts::UserSections::Memory64Feature;
      case FeatureSet::TypedFunctionReferences:
        return BinaryConsts::UserSections::TypedFunctionReferencesFeature;
      case FeatureSet::RelaxedSIMD:
        return BinaryConsts::UserSections::RelaxedSIMDFeature;
      default:
        WASM_UNREACHABLE("unexpected feature flag");
    }
  };

  std::vector<const char*> features;
  wasm->features.iterFeatures(
    [&](FeatureSet::Feature f) { features.push_back(toString(f)); });

  auto start = startSection(BinaryConsts::User);
  writeInlineString(BinaryConsts::UserSections::TargetFeatures);
  o << U32LEB(features.size());
  for (auto& f : features) {
    o << uint8_t(BinaryConsts::FeatureUsed);
    writeInlineString(f);
  }
  finishSection(start);
}