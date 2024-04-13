    void RafImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Reading RAF file " << io_->path() << "\n";
#endif
        if (io_->open() != 0) throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isRafType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "RAF");
        }

        clearMetadata();

        if (io_->seek(84,BasicIo::beg) != 0) throw Error(kerFailedToReadImageData);
        byte jpg_img_offset [4];
        if (io_->read(jpg_img_offset, 4) != 4) throw Error(kerFailedToReadImageData);
        byte jpg_img_length [4];
        if (io_->read(jpg_img_length, 4) != 4) throw Error(kerFailedToReadImageData);
        uint32_t jpg_img_off_u32 = Exiv2::getULong((const byte *) jpg_img_offset, bigEndian);
        uint32_t jpg_img_len_u32 = Exiv2::getULong((const byte *) jpg_img_length, bigEndian);

        enforce(Safe::add(jpg_img_off_u32, jpg_img_len_u32) <= io_->size(), kerCorruptedMetadata);

#if LONG_MAX < UINT_MAX
        enforce(jpg_img_off_u32 <= static_cast<uint32_t>(std::numeric_limits<long>::max()),
                kerCorruptedMetadata);
        enforce(jpg_img_len_u32 <= static_cast<uint32_t>(std::numeric_limits<long>::max()),
                kerCorruptedMetadata);
#endif

        long jpg_img_off = static_cast<long>(jpg_img_off_u32);
        long jpg_img_len = static_cast<long>(jpg_img_len_u32);

        enforce(jpg_img_len >= 12, kerCorruptedMetadata);

        DataBuf buf(jpg_img_len - 12);
        if (io_->seek(jpg_img_off + 12,BasicIo::beg) != 0) throw Error(kerFailedToReadImageData);
        io_->read(buf.pData_, buf.size_);
        if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);

        io_->seek(0,BasicIo::beg); // rewind

        ByteOrder bo = TiffParser::decode(exifData_,
                                          iptcData_,
                                          xmpData_,
                                          buf.pData_,
                                          static_cast<std::uint32_t>(buf.size_));

        exifData_["Exif.Image2.JPEGInterchangeFormat"] = getULong(jpg_img_offset, bigEndian);
        exifData_["Exif.Image2.JPEGInterchangeFormatLength"] = getULong(jpg_img_length, bigEndian);

        setByteOrder(bo);
    } // RafImage::readMetadata