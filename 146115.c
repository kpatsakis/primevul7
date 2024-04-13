    void ExifParser::encode(Blob &blob, ByteOrder byteOrder, const ExifData &exifData) {
        encode(blob, 0, 0, byteOrder, exifData);
    }