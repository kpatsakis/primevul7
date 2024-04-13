    void ExifData::sortByTag()
    {
        exifMetadata_.sort(cmpMetadataByTag);
    }