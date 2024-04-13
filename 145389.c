    long ExifThumbC::writeFile(const std::wstring& wpath) const
    {
        Thumbnail::UniquePtr thumbnail = Thumbnail::create(exifData_);
        if (thumbnail.get() == 0) return 0;
        std::wstring name = wpath + thumbnail->wextension();
        DataBuf buf(thumbnail->copy(exifData_));
        if (buf.size_ == 0) return 0;
        return Exiv2::writeFile(buf, name);
    }