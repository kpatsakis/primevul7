    int JpegBase::initImage(const byte initData[], long dataSize)
    {
        if (io_->open() != 0) {
            return 4;
        }
        IoCloser closer(*io_);
        if (io_->write(initData, dataSize) != static_cast<size_t>(dataSize)) {
            return 4;
        }
        return 0;
    }