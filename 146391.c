    int Erase::eraseExifData(Exiv2::Image* image) const
    {
        if (Params::instance().verbose_ && image->exifData().count() > 0) {
            std::cout << _("Erasing Exif data from the file") << std::endl;
        }
        image->clearExifData();
        return 0;
    }