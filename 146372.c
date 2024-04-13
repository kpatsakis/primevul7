    long ExifThumbC::writeFile(const std::string& path) const
    {
        Thumbnail::UniquePtr thumbnail = Thumbnail::create(exifData_);
        if (thumbnail.get() == 0) return 0;
        std::string name = path + thumbnail->extension();
        DataBuf buf(thumbnail->copy(exifData_));
        if (buf.size_ == 0) return 0;
        return Exiv2::writeFile(buf, name);
    }