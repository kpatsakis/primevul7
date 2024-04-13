    void TiffDecoder::visitMnEntry(TiffMnEntry* object)
    {
        // Always decode binary makernote tag
        decodeTiffEntry(object);
    }