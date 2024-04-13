    int Print::printComment()
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        assert(image.get() != 0);
        image->readMetadata();
        if (Params::instance().verbose_) {
            std::cout << _("JPEG comment") << ": ";
        }
        std::cout << image->comment() << std::endl;
        return 0;
    }