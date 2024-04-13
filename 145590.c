    void MrwImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Reading MRW file " << io_->path() << "\n";
#endif
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isMrwType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "MRW");
        }
        clearMetadata();

        // Find the TTW block and read it into a buffer
        uint32_t const len = 8;
        byte tmp[len];
        io_->read(tmp, len);
        uint32_t pos = len;
        uint32_t const end = getULong(tmp + 4, bigEndian);

        pos += len;
        if (pos > end) throw Error(kerFailedToReadImageData);
        io_->read(tmp, len);
        if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);

        while (memcmp(tmp + 1, "TTW", 3) != 0) {
            uint32_t const siz = getULong(tmp + 4, bigEndian);
            pos += siz;
            if (pos > end) throw Error(kerFailedToReadImageData);
            io_->seek(siz, BasicIo::cur);
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);

            pos += len;
            if (pos > end) throw Error(kerFailedToReadImageData);
            io_->read(tmp, len);
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
        }

        DataBuf buf(getULong(tmp + 4, bigEndian));
        io_->read(buf.pData_, buf.size_);
        if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);

        ByteOrder bo = TiffParser::decode(exifData_,
                                          iptcData_,
                                          xmpData_,
                                          buf.pData_,
                                          (uint32_t)buf.size_);
        setByteOrder(bo);
    } // MrwImage::readMetadata