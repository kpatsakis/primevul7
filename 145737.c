    int Extract::writePreviews() const
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        image->readMetadata();

        Exiv2::PreviewManager pvMgr(*image);
        Exiv2::PreviewPropertiesList pvList = pvMgr.getPreviewProperties();

        const Params::PreviewNumbers& numbers = Params::instance().previewNumbers_;
        for (auto n = numbers.cbegin(); n != numbers.cend(); ++n) {
            if (*n == 0) {
                // Write all previews
                for (int num = 0; num < static_cast<int>(pvList.size()); ++num) {
                    writePreviewFile(pvMgr.getPreviewImage(pvList[num]), num + 1);
                }
                break;
            }
            if (*n > static_cast<int>(pvList.size())) {
                std::cerr << path_ << ": " << _("Image does not have preview") << " " << *n << "\n";
                continue;
            }
            writePreviewFile(pvMgr.getPreviewImage(pvList[*n - 1]), *n);
        }
        return 0;
    }