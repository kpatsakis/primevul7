    int Adjust::run(const std::string& path) try {
        adjustment_ = Params::instance().adjustment_;
        yearAdjustment_ = Params::instance().yodAdjust_[Params::yodYear].adjustment_;
        monthAdjustment_ = Params::instance().yodAdjust_[Params::yodMonth].adjustment_;
        dayAdjustment_ = Params::instance().yodAdjust_[Params::yodDay].adjustment_;

        if (!Exiv2::fileExists(path, true)) {
            std::cerr << path << ": " << _("Failed to open the file\n");
            return -1;
        }
        Timestamp ts;
        if (Params::instance().preserve_)
            ts.read(path);

        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
        image->readMetadata();
        Exiv2::ExifData& exifData = image->exifData();
        if (exifData.empty()) {
            std::cerr << path << ": " << _("No Exif data found in the file\n");
            return -3;
        }
        int rc = adjustDateTime(exifData, "Exif.Image.DateTime", path);
        rc += adjustDateTime(exifData, "Exif.Photo.DateTimeOriginal", path);
        rc += adjustDateTime(exifData, "Exif.Image.DateTimeOriginal", path);
        rc += adjustDateTime(exifData, "Exif.Photo.DateTimeDigitized", path);

        if (rc == 0) {
            image->writeMetadata();
            if (Params::instance().preserve_)
                ts.touch(path);
        }
        return rc ? 1 : 0;
    } catch (const Exiv2::AnyError& e) {