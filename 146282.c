    ExifData::iterator ExifData::erase(ExifData::iterator beg, ExifData::iterator end)
    {
        return exifMetadata_.erase(beg, end);
    }