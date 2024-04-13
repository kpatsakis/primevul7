    int Print::printSummary()
    {
        if (!Exiv2::fileExists(path_, true)) {
            std::cerr << path_ << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path_);
        image->readMetadata();
        Exiv2::ExifData& exifData = image->exifData();
        align_ = 16;

        // Filename
        printLabel(_("File name"));
        std::cout << path_ << std::endl;

        // Filesize
        struct stat buf;
        if (0 == stat(path_.c_str(), &buf)) {
            printLabel(_("File size"));
            std::cout << buf.st_size << " " << _("Bytes") << std::endl;
        }

        // MIME type
        printLabel(_("MIME type"));
        std::cout << image->mimeType() << std::endl;

        // Image size
        printLabel(_("Image size"));
        std::cout << image->pixelWidth() << " x " << image->pixelHeight() << std::endl;

        if (exifData.empty()) {
            std::cerr << path_ << ": " << _("No Exif data found in the file\n");
            return -3;
        }

        // Camera make
        printTag(exifData, "Exif.Image.Make", _("Camera make"));

        // Camera model
        printTag(exifData, "Exif.Image.Model", _("Camera model"));

        // Image Timestamp
        printTag(exifData, "Exif.Photo.DateTimeOriginal", _("Image timestamp"));

        // Image number
        // Todo: Image number for cameras other than Canon
        printTag(exifData, "Exif.Canon.FileNumber", _("Image number"));

        // Exposure time
        // From ExposureTime, failing that, try ShutterSpeedValue
        printLabel(_("Exposure time"));
        {
            bool done = 0 != printTag(exifData, "Exif.Photo.ExposureTime");
            if (!done) {
                printTag(exifData, "Exif.Photo.ShutterSpeedValue");
            }
        }
        std::cout << std::endl;

        // Aperture
        // Get if from FNumber and, failing that, try ApertureValue
        {
            printLabel(_("Aperture"));
            {
                bool done = 0 != printTag(exifData, "Exif.Photo.FNumber");
                if (!done) {
                    printTag(exifData, "Exif.Photo.ApertureValue");
                }
            }
            std::cout << std::endl;

            // Exposure bias
            printTag(exifData, "Exif.Photo.ExposureBiasValue", _("Exposure bias"));

            // Flash
            printTag(exifData, "Exif.Photo.Flash", _("Flash"));

            // Flash bias
            printTag(exifData, Exiv2::flashBias, _("Flash bias"));

            // Actual focal length and 35 mm equivalent
            // Todo: Calculate 35 mm equivalent a la jhead
            printLabel(_("Focal length"));
            if (1 == printTag(exifData, "Exif.Photo.FocalLength")) {
                auto md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.FocalLengthIn35mmFilm"));
                if (md != exifData.end()) {
                    std::cout << " (" << _("35 mm equivalent") << ": " << md->print(&exifData) << ")";
                }
            } else {
                printTag(exifData, "Exif.Canon.FocalLength");
            }
            std::cout << std::endl;
        }

        // Subject distance
        {
            printLabel(_("Subject distance"));
            bool done = 0 != printTag(exifData, "Exif.Photo.SubjectDistance");
            if (!done) {
                printTag(exifData, "Exif.CanonSi.SubjectDistance");
                printTag(exifData, "Exif.CanonFi.FocusDistanceLower");
                printTag(exifData, "Exif.CanonFi.FocusDistanceUpper");
            }
            std::cout << std::endl;
        }

        // ISO speed
        printTag(exifData, Exiv2::isoSpeed, _("ISO speed"));

        // Exposure mode
        printTag(exifData, Exiv2::exposureMode, _("Exposure mode"));

        // Metering mode
        printTag(exifData, "Exif.Photo.MeteringMode", _("Metering mode"));

        // Macro mode
        printTag(exifData, Exiv2::macroMode, _("Macro mode"));

        // Image quality setting (compression)
        printTag(exifData, Exiv2::imageQuality, _("Image quality"));

        // Exif Resolution
        {
            printLabel(_("Exif Resolution"));
            long xdim = 0;
            long ydim = 0;
            if (image->mimeType() == "image/tiff") {
                xdim = image->pixelWidth();
                ydim = image->pixelHeight();
            } else {
                auto md = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageWidth"));
                if (md == exifData.end()) {
                    md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelXDimension"));
                }
                if (md != exifData.end() && md->count() > 0) {
                    xdim = md->toLong();
                }
                md = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageLength"));
                if (md == exifData.end()) {
                    md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelYDimension"));
                }
                if (md != exifData.end() && md->count() > 0) {
                    ydim = md->toLong();
                }
            }
            if (xdim != 0 && ydim != 0) {
                std::cout << xdim << " x " << ydim;
            }
            std::cout << std::endl;
        }

        // White balance
        printTag(exifData, Exiv2::whiteBalance, _("White balance"));

        // Thumbnail
        printLabel(_("Thumbnail"));
        Exiv2::ExifThumbC exifThumb(exifData);
        std::string thumbExt = exifThumb.extension();
        if (thumbExt.empty()) {
            std::cout << _("None");
        } else {
            Exiv2::DataBuf dataBuf = exifThumb.copy();
            if (dataBuf.size_ == 0) {
                std::cout << _("None");
            } else {
                std::cout << exifThumb.mimeType() << ", " << dataBuf.size_ << " " << _("Bytes");
            }
        }
        std::cout << std::endl;

        // Copyright
        printTag(exifData, "Exif.Image.Copyright", _("Copyright"));

        // Exif Comment
        printTag(exifData, "Exif.Photo.UserComment", _("Exif comment"));
        std::cout << std::endl;

        return 0;
    }