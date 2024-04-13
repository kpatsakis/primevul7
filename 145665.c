    int Insert::insertXmpPacket(const std::string& path, const std::string& xmpPath) const
    {
        int rc = 0;
        bool bStdin = xmpPath == "-";
        if (bStdin) {
            Exiv2::DataBuf xmpBlob;
            Params::instance().getStdin(xmpBlob);
            rc = insertXmpPacket(path, xmpBlob, true);
        } else {
            if (!Exiv2::fileExists(xmpPath, true)) {
                std::cerr << xmpPath << ": " << _("Failed to open the file\n");
                rc = -1;
            }
            if (rc == 0 && !Exiv2::fileExists(path, true)) {
                std::cerr << path << ": " << _("Failed to open the file\n");
                rc = -1;
            }
            if (rc == 0) {
                Exiv2::DataBuf xmpBlob = Exiv2::readFile(xmpPath);
                rc = insertXmpPacket(path, xmpBlob);
            }
        }
        return rc;

    }