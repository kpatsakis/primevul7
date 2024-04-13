    int Erase::eraseThumbnail(Exiv2::Image* image) const
    {
        Exiv2::ExifThumb exifThumb(image->exifData());
        std::string thumbExt = exifThumb.extension();
        if (thumbExt.empty()) {
            return 0;
        }
        exifThumb.erase();
        if (Params::instance().verbose_) {
            std::cout << _("Erasing thumbnail data") << std::endl;
        }
        return 0;
    }