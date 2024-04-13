    size_t OlympusMnHeader::write(IoWrapper& ioWrapper,
                                    ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // OlympusMnHeader::write