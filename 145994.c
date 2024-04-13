    void CrwImage::writeMetadata()
    {
#ifdef DEBUG
        std::cerr << "Writing CRW file " << io_->path() << "\n";
#endif
        // Read existing image
        DataBuf buf;
        if (io_->open() == 0) {
            IoCloser closer(*io_);
            // Ensure that this is the correct image type
            if (isCrwType(*io_, false)) {
                // Read the image into a memory buffer
                buf.alloc((long) io_->size());
                io_->read(buf.pData_, buf.size_);
                if (io_->error() || io_->eof()) {
                    buf.reset();
                }
            }
        }

        Blob blob;
        CrwParser::encode(blob, buf.pData_, (uint32_t)buf.size_, this);

        // Write new buffer to file
        MemIo::UniquePtr tempIo(new MemIo);
        assert(tempIo.get() != 0);
        tempIo->write((blob.size() > 0 ? &blob[0] : 0), static_cast<long>(blob.size()));
        io_->close();
        io_->transfer(*tempIo); // may throw

    } // CrwImage::writeMetadata