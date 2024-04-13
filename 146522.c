    void TiffDecoder::decodeStdTiffEntry(const TiffEntryBase* object)
    {
        assert(object != 0);
        ExifKey key(object->tag(), groupName(object->group()));
        key.setIdx(object->idx());
        exifData_.add(key, object->pValue());

    } // TiffDecoder::decodeTiffEntry