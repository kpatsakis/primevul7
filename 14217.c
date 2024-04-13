void WasmBinaryBuilder::readFeatures(size_t payloadLen) {
  wasm.hasFeaturesSection = true;

  auto sectionPos = pos;
  size_t numFeatures = getU32LEB();
  for (size_t i = 0; i < numFeatures; ++i) {
    uint8_t prefix = getInt8();

    bool disallowed = prefix == BinaryConsts::FeatureDisallowed;
    bool required = prefix == BinaryConsts::FeatureRequired;
    bool used = prefix == BinaryConsts::FeatureUsed;

    if (!disallowed && !required && !used) {
      throwError("Unrecognized feature policy prefix");
    }
    if (required) {
      std::cerr << "warning: required features in feature section are ignored";
    }

    Name name = getInlineString();
    if (pos > sectionPos + payloadLen) {
      throwError("ill-formed string extends beyond section");
    }

    FeatureSet feature;
    if (name == BinaryConsts::UserSections::AtomicsFeature) {
      feature = FeatureSet::Atomics;
    } else if (name == BinaryConsts::UserSections::BulkMemoryFeature) {
      feature = FeatureSet::BulkMemory;
    } else if (name == BinaryConsts::UserSections::ExceptionHandlingFeature) {
      feature = FeatureSet::ExceptionHandling;
    } else if (name == BinaryConsts::UserSections::MutableGlobalsFeature) {
      feature = FeatureSet::MutableGlobals;
    } else if (name == BinaryConsts::UserSections::TruncSatFeature) {
      feature = FeatureSet::TruncSat;
    } else if (name == BinaryConsts::UserSections::SignExtFeature) {
      feature = FeatureSet::SignExt;
    } else if (name == BinaryConsts::UserSections::SIMD128Feature) {
      feature = FeatureSet::SIMD;
    } else if (name == BinaryConsts::UserSections::TailCallFeature) {
      feature = FeatureSet::TailCall;
    } else if (name == BinaryConsts::UserSections::ReferenceTypesFeature) {
      feature = FeatureSet::ReferenceTypes;
    } else if (name == BinaryConsts::UserSections::MultivalueFeature) {
      feature = FeatureSet::Multivalue;
    } else if (name == BinaryConsts::UserSections::GCFeature) {
      feature = FeatureSet::GC;
    } else if (name == BinaryConsts::UserSections::Memory64Feature) {
      feature = FeatureSet::Memory64;
    } else if (name ==
               BinaryConsts::UserSections::TypedFunctionReferencesFeature) {
      feature = FeatureSet::TypedFunctionReferences;
    } else if (name == BinaryConsts::UserSections::RelaxedSIMDFeature) {
      feature = FeatureSet::RelaxedSIMD;
    } else {
      // Silently ignore unknown features (this may be and old binaryen running
      // on a new wasm).
    }

    if (disallowed && wasm.features.has(feature)) {
      std::cerr
        << "warning: feature " << feature.toString()
        << " was enabled by the user, but disallowed in the features section.";
    }
    if (required || used) {
      wasm.features.enable(feature);
    }
  }
  if (pos != sectionPos + payloadLen) {
    throwError("bad features section size");
  }
}