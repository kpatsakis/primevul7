    int FixCom::run(const std::string& path)
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
            auto pos = exifData.findKey(Exiv2::ExifKey("Exif.Photo.UserComment"));
            if (pos == exifData.end()) {
                if (Params::instance().verbose_) {
                    std::cout << _("No Exif user comment found") << "\n";
                }
                return 0;
            }
            Exiv2::Value::UniquePtr v = pos->getValue();
            const Exiv2::CommentValue* pcv = dynamic_cast<const Exiv2::CommentValue*>(v.get());
            if (!pcv) {
                if (Params::instance().verbose_) {
                    std::cout << _("Found Exif user comment with unexpected value type") << "\n";
                }
                return 0;
            }
            Exiv2::CommentValue::CharsetId csId = pcv->charsetId();
            if (csId != Exiv2::CommentValue::unicode) {
                if (Params::instance().verbose_) {
                    std::cout << _("No Exif UNICODE user comment found") << "\n";
                }
                return 0;
            }
            std::string comment = pcv->comment(Params::instance().charset_.c_str());
            if (Params::instance().verbose_) {
                std::cout << _("Setting Exif UNICODE user comment to") << " \"" << comment << "\"\n";
            }
            comment = std::string("charset=\"") + Exiv2::CommentValue::CharsetInfo::name(csId) + "\" " + comment;
            // Remove BOM and convert value from source charset to UCS-2, but keep byte order
            pos->setValue(comment);
            image->writeMetadata();
            if (Params::instance().preserve_)
                ts.touch(path);

            return 0;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in fixcom action for file " << path << ":\n" << e << "\n";
            return 1;
        }
    }