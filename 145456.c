    const wchar_t* ExifThumbC::wextension() const
    {
        Thumbnail::UniquePtr thumbnail = Thumbnail::create(exifData_);
        if (thumbnail.get() == 0) return EXV_WIDEN("");
        return thumbnail->wextension();
    }