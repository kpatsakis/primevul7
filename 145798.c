    void CrwImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Reading CRW file " << io_->path() << "\n";
#endif
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isCrwType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotACrwImage);
        }
        clearMetadata();
        DataBuf file( (long) io().size());
        io_->read(file.pData_,file.size_);

        CrwParser::decode(this, io_->mmap(), (uint32_t) io_->size());

    } // CrwImage::readMetadata