    DataBuf PreviewImage::copy() const
    {
        return DataBuf(pData_, size_);
    }