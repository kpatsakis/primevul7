    uint32_t TiffImageEntry::doWriteData(IoWrapper& ioWrapper,
                                         ByteOrder byteOrder,
                                         int32_t   /*offset*/,
                                         uint32_t  /*dataIdx*/,
                                         uint32_t& /*imageIdx*/) const
    {
        uint32_t len = 0;
        // For makernotes, write TIFF image data to the data area
        if (group() > mnId) { // Todo: FIX THIS HACK!!!
            len = writeImage(ioWrapper, byteOrder);
        }
        return len;
    } // TiffImageEntry::doWriteData