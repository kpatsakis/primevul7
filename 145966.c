    size_t FujiMnHeader::write(IoWrapper& ioWrapper,
                                 ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // FujiMnHeader::write