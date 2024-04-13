    bool TiffEncoder::dirty() const
    {
        if (dirty_ || exifData_.count() > 0) return true;
        return false;
    }