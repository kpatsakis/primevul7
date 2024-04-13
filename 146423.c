    PreviewProperties Loader::getProperties() const
    {
        PreviewProperties prop;
        prop.id_ = id_;
        prop.size_ = (uint32_t)size_;
        prop.width_ = width_;
        prop.height_ = height_;
        return prop;
    }