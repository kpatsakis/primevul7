    int Insert::insertIccProfile(const std::string& path, Exiv2::DataBuf& iccProfileBlob) const
    {
        int rc = 0;
        // test path exists
        if (!Exiv2::fileExists(path, true)) {
            std::cerr << path << ": " << _("Failed to open the file\n");
            rc = -1;
        }

        // read in the metadata
        if (rc == 0) {
            Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
            image->readMetadata();
            // clear existing profile, assign the blob and rewrite image
            image->clearIccProfile();
            if (iccProfileBlob.size_) {
                image->setIccProfile(iccProfileBlob);
            }
            image->writeMetadata();
        }

        return rc;
    }