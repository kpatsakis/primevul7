    size_t PentaxMnHeader::write(IoWrapper& ioWrapper,
                                   ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // PentaxMnHeader::write