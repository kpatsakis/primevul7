    XmpSidecar::XmpSidecar(BasicIo::UniquePtr io, bool create)
        : Image(ImageType::xmp, mdXmp, std::move(io))
    {
        if (create) {
            if (io_->open() == 0) {
                IoCloser closer(*io_);
                io_->write(reinterpret_cast<const byte*>(xmlHeader), xmlHdrCnt);
            }
        }
    } // XmpSidecar::XmpSidecar