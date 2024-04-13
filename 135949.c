    CompressionHeader(int32_t _opcode, int32_t _size, uint8_t _id)
        : originalOpCode{_opcode}, uncompressedSize{_size}, compressorId{_id} {}