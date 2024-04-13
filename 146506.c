    int Insert::insertThumbnail(const std::string& path) const
    {
        std::string thumbPath = newFilePath(path, "-thumb.jpg");
        if (!Exiv2::fileExists(thumbPath, true)) {
            std::cerr << thumbPath << ": " << _("Failed to open the file\n");
            return -1;
        }
        if (!Exiv2::fileExists(path, true)) {
            std::cerr << path << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
        image->readMetadata();
        Exiv2::ExifThumb exifThumb(image->exifData());
        exifThumb.setJpegThumbnail(thumbPath);
        image->writeMetadata();

        return 0;
    }