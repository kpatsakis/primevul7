    int Extract::writeIccProfile(const std::string& target) const
    {
        int rc = 0;
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            rc = -1;
        }

        bool bStdout = target == "-";

        if (rc == 0) {
            Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
            image->readMetadata();
            if (!image->iccProfileDefined()) {
                std::cerr << _("No embedded iccProfile: ") << path_ << std::endl;
                rc = -2;
            } else {
                if (bStdout) {  // -eC-
                    std::cout.write((const char*)image->iccProfile()->pData_, image->iccProfile()->size_);
                } else {
                    if (Params::instance().verbose_) {
                        std::cout << _("Writing iccProfile: ") << target << std::endl;
                    }
                    Exiv2::FileIo iccFile(target);
                    iccFile.open("wb");
                    iccFile.write(image->iccProfile()->pData_, image->iccProfile()->size_);
                    iccFile.close();
                }
            }
        }
        return rc;
    }