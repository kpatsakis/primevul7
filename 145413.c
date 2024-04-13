    uint32_t TiffSubIfd::doSizeData() const
    {
        uint32_t len = 0;
        for (Ifds::const_iterator i = ifds_.begin(); i != ifds_.end(); ++i) {
            len += (*i)->size();
        }
        return len;
    } // TiffSubIfd::doSizeData