    int Erase::run(const std::string& path) try {
        path_ = path;

        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Timestamp ts;
        if (Params::instance().preserve_)
            ts.read(path);

        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        image->readMetadata();
        // Thumbnail must be before Exif
        int rc = 0;
        if (Params::instance().target_ & Params::ctThumb) {
            rc = eraseThumbnail(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctExif) {
            rc = eraseExifData(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctIptc) {
            rc = eraseIptcData(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctComment) {
            rc = eraseComment(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctXmp) {
            rc = eraseXmpData(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctIccProfile) {
            rc = eraseIccProfile(image.get());
        }
        if (0 == rc && Params::instance().target_ & Params::ctIptcRaw) {
            rc = printStructure(std::cout, Exiv2::kpsIptcErase, path_);
        }

        if (0 == rc) {
            image->writeMetadata();
            if (Params::instance().preserve_)
                ts.touch(path);
        }

        return rc;
    } catch (const Exiv2::AnyError& e) {