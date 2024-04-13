    uint32_t TiffMnEntry::doCount() const
    {
        if (!mn_) {
            return TiffEntryBase::doCount();
        }
        // Count of IFD makernote in tag Exif.Photo.MakerNote is the size of the
        // Makernote in bytes
        assert(tiffType() == ttUndefined || tiffType() == ttUnsignedByte || tiffType() == ttSignedByte);
        return mn_->size();
    }