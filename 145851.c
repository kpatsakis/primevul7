    const char* ExifThumbC::extension() const
    {
        Thumbnail::UniquePtr thumbnail = Thumbnail::create(exifData_);
        if (thumbnail.get() == 0) return "";
        return thumbnail->extension();
    }