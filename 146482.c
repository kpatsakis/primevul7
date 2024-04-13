    void XmpSidecar::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Reading XMP file " << io_->path() << "\n";
#endif
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isXmpType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "XMP");
        }
        // Read the XMP packet from the IO stream
        std::string xmpPacket;
        const long len = 64 * 1024;
        byte buf[len];
        size_t l;
        while ((l = io_->read(buf, len)) > 0) {
            xmpPacket.append(reinterpret_cast<char*>(buf), l);
        }
        if (io_->error()) throw Error(kerFailedToReadImageData);
        clearMetadata();
        xmpPacket_ = xmpPacket;
        if (xmpPacket_.size() > 0 && XmpParser::decode(xmpData_, xmpPacket_)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to decode XMP metadata.\n";
#endif
        }

        // #1112 - store dates to deal with loss of TZ information during conversions
        for (Exiv2::XmpData::const_iterator it = xmpData_.begin(); it != xmpData_.end(); ++it) {
            std::string  key(it->key());
            if ( key.find("Date") != std::string::npos ) {
                std::string value(it->value().toString());
                dates_[key] = value;
            }
        }

        copyXmpToIptc(xmpData_, iptcData_);
        copyXmpToExif(xmpData_, exifData_);
    } // XmpSidecar::readMetadata