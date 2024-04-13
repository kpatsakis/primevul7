    void ExifData::add(const Exifdatum& exifdatum)
    {
        // allow duplicates
        exifMetadata_.push_back(exifdatum);
    }