    DataBuf LoaderXmpJpeg::getData() const
    {
        if (!valid()) return DataBuf();
        return DataBuf(preview_.pData_, preview_.size_);
    }