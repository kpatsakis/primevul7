    TiffHeader::TiffHeader(ByteOrder byteOrder, uint32_t offset, bool hasImageTags)
        : TiffHeaderBase(42, 8, byteOrder, offset),
          hasImageTags_(hasImageTags)
    {
    }