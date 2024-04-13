    void PgfImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Exiv2::PgfImage::readMetadata: Reading PGF file " << io_->path() << "\n";
#endif
        if (io_->open() != 0)
        {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isPgfType(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "PGF");
        }
        clearMetadata();

        readPgfMagicNumber(*io_);

        uint32_t headerSize = readPgfHeaderSize(*io_);
        readPgfHeaderStructure(*io_, pixelWidth_, pixelHeight_);

        // And now, the most interresting, the user data byte array where metadata are stored as small image.

        enforce(headerSize <= std::numeric_limits<uint32_t>::max() - 8, kerCorruptedMetadata);
#if LONG_MAX < UINT_MAX
        enforce(headerSize + 8 <= static_cast<uint32_t>(std::numeric_limits<long>::max()),
                kerCorruptedMetadata);
#endif
        long size = static_cast<long>(headerSize) + 8 - io_->tell();

#ifdef DEBUG
        std::cout << "Exiv2::PgfImage::readMetadata: Found Image data (" << size << " bytes)\n";
#endif

        if (size < 0 || static_cast<size_t>(size) > io_->size()) throw Error(kerInputDataReadFailed);
        if (size == 0) return;

        DataBuf imgData(size);
        std::memset(imgData.pData_, 0x0, imgData.size_);
        size_t bufRead = io_->read(imgData.pData_, imgData.size_);
        if (io_->error()) throw Error(kerFailedToReadImageData);
        if (bufRead != imgData.size_) throw Error(kerInputDataReadFailed);

        Image::UniquePtr image = Exiv2::ImageFactory::open(imgData.pData_, imgData.size_);
        image->readMetadata();
        exifData() = image->exifData();
        iptcData() = image->iptcData();
        xmpData()  = image->xmpData();

    } // PgfImage::readMetadata