    DataBuf LoaderNative::getData() const
    {
        if (!valid()) return DataBuf();

        BasicIo &io = image_.io();
        if (io.open() != 0) {
            throw Error(kerDataSourceOpenFailed, io.path(), strError());
        }
        IoCloser closer(io);
        const byte* data = io.mmap();
        if ((long)io.size() < nativePreview_.position_ + static_cast<long>(nativePreview_.size_)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Invalid native preview position or size.\n";
#endif
            return DataBuf();
        }
        if (nativePreview_.filter_ == "") {
            return DataBuf(data + nativePreview_.position_, static_cast<long>(nativePreview_.size_));
        } else if (nativePreview_.filter_ == "hex-ai7thumbnail-pnm") {
            const DataBuf ai7thumbnail = decodeHex(data + nativePreview_.position_, static_cast<long>(nativePreview_.size_));
            const DataBuf rgb = decodeAi7Thumbnail(ai7thumbnail);
            return makePnm(width_, height_, rgb);
        } else if (nativePreview_.filter_ == "hex-irb") {
            const DataBuf psData = decodeHex(data + nativePreview_.position_, static_cast<long>(nativePreview_.size_));
            const byte *record;
            uint32_t sizeHdr;
            uint32_t sizeData;
            if (Photoshop::locatePreviewIrb(psData.pData_, psData.size_, &record, &sizeHdr, &sizeData) != 0) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Missing preview IRB in Photoshop EPS preview.\n";
#endif
                return DataBuf();
            }
            return DataBuf(record + sizeHdr + 28, sizeData - 28);
        } else {
            throw Error(kerErrorMessage, "Invalid native preview filter: " + nativePreview_.filter_);
        }
    }