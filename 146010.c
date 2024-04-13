    int Print::printPreviewList()
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        assert(image.get() != 0);
        image->readMetadata();
        bool const manyFiles = Params::instance().files_.size() > 1;
        int cnt = 0;
        Exiv2::PreviewManager pm(*image);
        Exiv2::PreviewPropertiesList list = pm.getPreviewProperties();
        for (auto pos = list.begin(); pos != list.end(); ++pos) {
            if (manyFiles) {
                std::cout << std::setfill(' ') << std::left << std::setw(20) << path_ << "  ";
            }
            std::cout << _("Preview") << " " << ++cnt << ": " << pos->mimeType_ << ", ";
            if (pos->width_ != 0 && pos->height_ != 0) {
                std::cout << pos->width_ << "x" << pos->height_ << " " << _("pixels") << ", ";
            }
            std::cout << pos->size_ << " " << _("bytes") << "\n";
        }
        return 0;
    }