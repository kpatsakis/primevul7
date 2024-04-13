    size_t IoWrapper::write(const byte* pData, size_t wcount)
    {
        if (!wroteHeader_ && wcount > 0) {
            io_.write(pHeader_, size_);
            wroteHeader_ = true;
        }
        return io_.write(pData, wcount);
    }