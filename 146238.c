    uint32_t fillGap(Exiv2::Internal::IoWrapper& ioWrapper, uint32_t curr, uint32_t tobe)
    {
        if (curr < tobe) {
            Exiv2::DataBuf buf(tobe - curr);
            memset(buf.pData_, 0x0, buf.size_);
            ioWrapper.write(buf.pData_, buf.size_);
            return tobe - curr;
        }
        return 0;

    } // fillGap