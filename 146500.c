    int Rename::run(const std::string& path)
    {
        try {
            if (!Exiv2::fileExists(path, true)) {
                std::cerr << path << ": " << _("Failed to open the file\n");
                return -1;
            }
            Timestamp ts;
            if (Params::instance().preserve_)
                ts.read(path);

            Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
            image->readMetadata();
            Exiv2::ExifData& exifData = image->exifData();
            if (exifData.empty()) {
                std::cerr << path << ": " << _("No Exif data found in the file\n");
                return -3;
            }

            auto md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.DateTimeOriginal"));
            if (md == exifData.end()) {
                md = exifData.findKey(Exiv2::ExifKey("Exif.Image.DateTime"));
            }
            if (md == exifData.end()) {
                std::cerr << _("Neither tag") << " `Exif.Photo.DateTimeOriginal' " << _("nor")
                          << " `Exif.Image.DateTime' " << _("found in the file") << " " << path << "\n";
                return 1;
            }

            std::string dateTime = md->toString();
            if (dateTime.length() == 0 || dateTime[0] == ' ') {
                std::cerr << _("Image file creation timestamp not set in the file") << " " << path << "\n";
                return 1;
            }

            struct tm tm;
            if (str2Tm(dateTime, &tm) != 0) {
                std::cerr << _("Failed to parse timestamp") << " `" << dateTime << "' " << _("in the file") << " "
                          << path << "\n";
                return 1;
            }

            if (Params::instance().timestamp_ || Params::instance().timestampOnly_) {
                ts.read(&tm);
            }
            int rc = 0;
            std::string newPath = path;
            if (Params::instance().timestampOnly_) {
                if (Params::instance().verbose_) {
                    std::cout << _("Updating timestamp to") << " " << dateTime << std::endl;
                }
            } else {
                rc = renameFile(newPath, &tm);
                if (rc == -1)
                    return 0;  // skip
            }
            if (0 == rc &&
                (Params::instance().preserve_ || Params::instance().timestamp_ || Params::instance().timestampOnly_)) {
                ts.touch(newPath);
            }
            return rc;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in rename action for file " << path << ":\n" << e << "\n";
            return 1;
        }
    }