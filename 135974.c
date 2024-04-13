    static size_t size() {
        return sizeof(originalOpCode) + sizeof(uncompressedSize) + sizeof(compressorId);
    }