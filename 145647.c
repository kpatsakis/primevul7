    size_t SigmaMnHeader::write(IoWrapper& ioWrapper,
                                  ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature1_, sizeOfSignature());
        return sizeOfSignature();
    } // SigmaMnHeader::write