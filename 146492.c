    DataBuf ExifThumbC::copy() const
    {
        Thumbnail::UniquePtr thumbnail = Thumbnail::create(exifData_);
        if (thumbnail.get() == 0) return DataBuf();
        return thumbnail->copy(exifData_);
    }