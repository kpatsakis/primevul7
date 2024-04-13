    int IoWrapper::putb(byte data)
    {
        if (!wroteHeader_) {
            io_.write(pHeader_, size_);
            wroteHeader_ = true;
        }
        return io_.putb(data);
    }