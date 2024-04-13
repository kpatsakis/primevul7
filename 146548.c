    size_t Nikon2MnHeader::write(IoWrapper& ioWrapper,
                                   ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // Nikon2MnHeader::write