    int Insert::insertIccProfile(const std::string& path, const std::string& iccPath) const
    {
        int rc = 0;
        // for path "foo.XXX", do a binary copy of "foo.icc"
        std::string iccProfilePath = newFilePath(path, ".icc");
        if (iccPath == "-") {
            Exiv2::DataBuf iccProfile;
            Params::instance().getStdin(iccProfile);
            rc = insertIccProfile(path, iccProfile);
        } else {
            if (!Exiv2::fileExists(iccProfilePath, true)) {
                std::cerr << iccProfilePath << ": " << _("Failed to open the file\n");
                rc = -1;
            } else {
                Exiv2::DataBuf iccProfile = Exiv2::readFile(iccPath);
                rc = insertIccProfile(path, iccProfile);
            }
        }
        return rc;
    }