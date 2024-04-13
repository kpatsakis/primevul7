    int Erase::eraseXmpData(Exiv2::Image* image) const
    {
        if (Params::instance().verbose_ && image->xmpData().count() > 0) {
            std::cout << _("Erasing XMP data from the file") << std::endl;
        }
        image->clearXmpData();  // Quick fix for bug #612
        image->clearXmpPacket();
        return 0;
    }