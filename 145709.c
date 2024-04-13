    int Print::printList()
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        assert(image.get() != 0);
        image->readMetadata();
        // Set defaults for metadata types and data columns
        if (Params::instance().printTags_ == Exiv2::mdNone) {
            Params::instance().printTags_ = Exiv2::mdExif | Exiv2::mdIptc | Exiv2::mdXmp;
        }
        if (Params::instance().printItems_ == 0) {
            Params::instance().printItems_ = Params::prKey | Params::prType | Params::prCount | Params::prTrans;
        }
        return printMetadata(image.get());
    }