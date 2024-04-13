    uint32_t TiffSubIfd::doSize() const
    {
        return static_cast<uint32_t>(ifds_.size()) * 4;
    } // TiffSubIfd::doSize