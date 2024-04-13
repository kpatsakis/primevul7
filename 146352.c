    void ExifData::sortByKey()
    {
        exifMetadata_.sort(cmpMetadataByKey);
    }