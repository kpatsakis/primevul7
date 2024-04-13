    PreviewImage::PreviewImage(const PreviewImage& rhs)
    {
        properties_ = rhs.properties_;
        pData_ = new byte[rhs.size_];
        memcpy(pData_, rhs.pData_, rhs.size_);
        size_ = rhs.size_;
    }