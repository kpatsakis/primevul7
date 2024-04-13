    size_t PentaxDngMnHeader::write(IoWrapper& ioWrapper,
                                   ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // PentaxDngMnHeader::write