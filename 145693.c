    int FixIso::run(const std::string& path)
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
            auto md = Exiv2::isoSpeed(exifData);
            if (md != exifData.end()) {
                if (strcmp(md->key().c_str(), "Exif.Photo.ISOSpeedRatings") == 0) {
                    if (Params::instance().verbose_) {
                        std::cout << _("Standard Exif ISO tag exists; not modified\n");
                    }
                    return 0;
                }
                // Copy the proprietary tag to the standard place
                std::ostringstream os;
                md->write(os, &exifData);
                if (Params::instance().verbose_) {
                    std::cout << _("Setting Exif ISO value to") << " " << os.str() << "\n";
                }
                exifData["Exif.Photo.ISOSpeedRatings"] = os.str();
            }
            image->writeMetadata();
            if (Params::instance().preserve_)
                ts.touch(path);

            return 0;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in fixiso action for file " << path << ":\n" << e << "\n";
            return 1;
        }
    }