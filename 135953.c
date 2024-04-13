    CompressionHeader(ConstDataRangeCursor* cursor) {
        originalOpCode = cursor->readAndAdvance<LittleEndian<std::int32_t>>();
        uncompressedSize = cursor->readAndAdvance<LittleEndian<std::int32_t>>();
        compressorId = cursor->readAndAdvance<LittleEndian<uint8_t>>();
    }