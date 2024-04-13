    PreviewImage& PreviewImage::operator=(const PreviewImage& rhs)
    {
        if (this == &rhs) return *this;
        if (rhs.size_ > size_) {
            delete[] pData_;
            pData_ = new byte[rhs.size_];
        }
        properties_ = rhs.properties_;
        memcpy(pData_, rhs.pData_, rhs.size_);
        size_ = rhs.size_;
        return *this;
    }