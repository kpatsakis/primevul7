    PreviewImage::PreviewImage(const PreviewProperties& properties, DataBuf data)
        : properties_(properties)
    {
        pData_ = data.pData_;
        size_ = (uint32_t)data.size_;
        std::pair<byte*, long> ret = data.release();
        UNUSED(ret);
    }