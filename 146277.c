    int Extract::writeThumbnail() const
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        image->readMetadata();
        Exiv2::ExifData& exifData = image->exifData();
        if (exifData.empty()) {
            std::cerr << path_ << ": " << _("No Exif data found in the file\n");
            return -3;
        }
        int rc = 0;
        Exiv2::ExifThumb exifThumb(exifData);
        std::string thumbExt = exifThumb.extension();
        if (thumbExt.empty()) {
            std::cerr << path_ << ": " << _("Image does not contain an Exif thumbnail\n");
        } else {
            std::string thumb = newFilePath(path_, "-thumb");
            std::string thumbPath = thumb + thumbExt;
            if (dontOverwrite(thumbPath))
                return 0;
            if (Params::instance().verbose_) {
                Exiv2::DataBuf buf = exifThumb.copy();
                if (buf.size_ != 0) {
                    std::cout << _("Writing thumbnail") << " (" << exifThumb.mimeType() << ", " << buf.size_ << " "
                              << _("Bytes") << ") " << _("to file") << " " << thumbPath << std::endl;
                }
            }
            rc = static_cast<int>(exifThumb.writeFile(thumb));
            if (rc == 0) {
                std::cerr << path_ << ": " << _("Exif data doesn't contain a thumbnail\n");
            }
        }
        return rc;
    }