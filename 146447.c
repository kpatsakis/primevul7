    size_t Casio2MnHeader::write(IoWrapper& ioWrapper,
                                 ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // Casio2MnHeader::write