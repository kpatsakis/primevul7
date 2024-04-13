    int Extract::run(const std::string& path)
    {
        try {
            path_ = path;
            int rc = 0;

            bool bStdout = Params::instance().target_ & Params::ctStdInOut ? true : false;
            if (bStdout) {
                _setmode(_fileno(stdout), _O_BINARY);
            }

            if (Params::instance().target_ & Params::ctThumb) {
                rc = writeThumbnail();
            }
            if (!rc && Params::instance().target_ & Params::ctPreview) {
                rc = writePreviews();
            }
            if (!rc && Params::instance().target_ & Params::ctXmpSidecar) {
                std::string xmpPath = bStdout ? "-" : newFilePath(path_, ".xmp");
                if (dontOverwrite(xmpPath))
                    return 0;
                rc = metacopy(path_, xmpPath, Exiv2::ImageType::xmp, false);
            }
            if (!rc && Params::instance().target_ & Params::ctIccProfile) {
                std::string iccPath = bStdout ? "-" : newFilePath(path_, ".icc");
                rc = writeIccProfile(iccPath);
            }
            if (!rc && !(Params::instance().target_ & Params::ctXmpSidecar) &&
                !(Params::instance().target_ & Params::ctThumb) && !(Params::instance().target_ & Params::ctPreview) &&
                !(Params::instance().target_ & Params::ctIccProfile)) {
                std::string exvPath = bStdout ? "-" : newFilePath(path_, ".exv");
                if (dontOverwrite(exvPath))
                    return 0;
                rc = metacopy(path_, exvPath, Exiv2::ImageType::exv, false);
            }
            return rc;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in extract action for file " << path << ":\n" << e << "\n";
            return 1;
        }
    }