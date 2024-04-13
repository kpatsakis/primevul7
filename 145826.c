    uint32_t TiffDirectory::doSizeImage() const
    {
        uint32_t len = 0;
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            len += (*i)->sizeImage();
        }
        if (pNext_) {
            len += pNext_->sizeImage();
        }
        return len;
    } // TiffDirectory::doSizeImage