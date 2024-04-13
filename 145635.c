    DataBuf LoaderTiff::getData() const
    {
        const ExifData &exifData = image_.exifData();

        ExifData preview;

        // copy tags
        for (ExifData::const_iterator pos = exifData.begin(); pos != exifData.end(); ++pos) {
            if (pos->groupName() == group_) {
                /*
                   Write only the necessary TIFF image tags
                   tags that especially could cause problems are:
                   "NewSubfileType" - the result is no longer a thumbnail, it is a standalone image
                   "Orientation" - this tag typically appears only in the "Image" group. Deleting it ensures
                                   consistent result for all previews, including JPEG
                */
                uint16_t tag = pos->tag();
                if (tag != 0x00fe && tag != 0x00ff && Internal::isTiffImageTag(tag, Internal::ifd0Id)) {
                    preview.add(ExifKey(tag, "Image"), &pos->value());
                }
            }
        }

        Value &dataValue = const_cast<Value&>(preview["Exif.Image." + offsetTag_].value());

        if (dataValue.sizeDataArea() == 0) {
            // image data are not available via exifData, read them from image_.io()
            BasicIo &io = image_.io();

            if (io.open() != 0) {
                throw Error(kerDataSourceOpenFailed, io.path(), strError());
            }
            IoCloser closer(io);

            const Exiv2::byte* base = io.mmap();

            const Value &sizes = preview["Exif.Image." + sizeTag_].value();

            if (sizes.count() == dataValue.count()) {
                if (sizes.count() == 1) {
                    // this saves one copying of the buffer
                    uint32_t offset = dataValue.toLong(0);
                    uint32_t size = sizes.toLong(0);
                    if (Safe::add(offset, size) <= static_cast<uint32_t>(io.size()))
                        dataValue.setDataArea(base + offset, size);
                }
                else {
                    // FIXME: the buffer is probably copied twice, it should be optimized
                    enforce(size_ <= static_cast<uint32_t>(io.size()), kerCorruptedMetadata);
                    DataBuf buf(size_);
                    uint32_t idxBuf = 0;
                    for (int i = 0; i < sizes.count(); i++) {
                        uint32_t offset = dataValue.toLong(i);
                        uint32_t size = sizes.toLong(i);
                        enforce(Safe::add(idxBuf, size) < size_, kerCorruptedMetadata);
                        if (size!=0 && Safe::add(offset, size) <= static_cast<uint32_t>(io.size()))
                            memcpy(&buf.pData_[idxBuf], base + offset, size);
                        idxBuf += size;
                    }
                    dataValue.setDataArea(buf.pData_, (uint32_t)buf.size_);
                }
            }
        }

        // Fix compression value in the CR2 IFD2 image
        if (0 == strcmp(group_, "Image2") && image_.mimeType() == "image/x-canon-cr2") {
            preview["Exif.Image.Compression"] = uint16_t(1);
        }

        // write new image
        MemIo mio;
        IptcData emptyIptc;
        XmpData  emptyXmp;
        TiffParser::encode(mio, 0, 0, Exiv2::littleEndian, preview, emptyIptc, emptyXmp);
        return DataBuf(mio.mmap(), (long) mio.size());
    }