    uint32_t TiffComponent::writeImage(IoWrapper& ioWrapper,
                                       ByteOrder byteOrder) const
    {
        return doWriteImage(ioWrapper, byteOrder);
    } // TiffComponent::writeImage