    uint32_t TiffDirectory::doWriteImage(IoWrapper& ioWrapper,
                                         ByteOrder byteOrder) const
    {
        uint32_t len = 0;
        TiffComponent* pSubIfd = 0;
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            if ((*i)->tag() == 0x014a) {
                // Hack: delay writing of sub-IFD image data to get the order correct
                assert(pSubIfd == 0);
                pSubIfd = *i;
                continue;
            }
            len += (*i)->writeImage(ioWrapper, byteOrder);
        }
        if (pSubIfd) {
            len += pSubIfd->writeImage(ioWrapper, byteOrder);
        }
        if (pNext_) {
            len += pNext_->writeImage(ioWrapper, byteOrder);
        }
        return len;
    } // TiffDirectory::doWriteImage