    int Modify::run(const std::string& path)
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

            int rc = applyCommands(image.get());

            // Save both exif and iptc metadata
            image->writeMetadata();

            if (Params::instance().preserve_)
                ts.touch(path);

            return rc;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in modify action for file " << path << ":\n" << e << "\n";
            return 1;
        }
    }