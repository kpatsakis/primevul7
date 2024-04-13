    size_t SonyMnHeader::write(IoWrapper& ioWrapper,
                                 ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // SonyMnHeader::write