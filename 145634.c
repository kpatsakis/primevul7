    size_t Olympus2MnHeader::write(IoWrapper& ioWrapper,
                                    ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // Olympus2MnHeader::write