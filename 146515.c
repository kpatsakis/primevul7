    int Insert::run(const std::string& path) try {
        // -i{tgt}-  reading from stdin?
        bool bStdin = (Params::instance().target_ & Params::ctStdInOut) ? true : false;

        if (!Exiv2::fileExists(path, true)) {
            std::cerr << path << ": " << _("Failed to open the file\n");
            return -1;
        }

        int rc = 0;
        Timestamp ts;
        if (Params::instance().preserve_)
            ts.read(path);

        if (Params::instance().target_ & Params::ctThumb) {
            rc = insertThumbnail(path);
        }

        if (rc == 0 && !(Params::instance().target_ & Params::ctXmpRaw) &&
            (Params::instance().target_ & Params::ctExif || Params::instance().target_ & Params::ctIptc ||
             Params::instance().target_ & Params::ctComment || Params::instance().target_ & Params::ctXmp)) {
            std::string suffix = Params::instance().suffix_;
            if (suffix.empty())
                suffix = ".exv";
            if (Params::instance().target_ & Params::ctXmpSidecar)
                suffix = ".xmp";
            std::string exvPath = bStdin ? "-" : newFilePath(path, suffix);
            rc = metacopy(exvPath, path, Exiv2::ImageType::none, true);
        }

        if (0 == rc && (Params::instance().target_ & (Params::ctXmpSidecar | Params::ctXmpRaw))) {
            std::string xmpPath = bStdin ? "-" : newFilePath(path, ".xmp");
            rc = insertXmpPacket(path, xmpPath);
        }

        if (0 == rc && Params::instance().target_ & Params::ctIccProfile) {
            std::string iccPath = bStdin ? "-" : newFilePath(path, ".icc");
            rc = insertIccProfile(path, iccPath);
        }

        if (Params::instance().preserve_)
            ts.touch(path);
        return rc;
    } catch (const Exiv2::AnyError& e) {