    size_t PanasonicMnHeader::write(IoWrapper& ioWrapper,
                                      ByteOrder /*byteOrder*/) const
    {
        ioWrapper.write(signature_, sizeOfSignature());
        return sizeOfSignature();
    } // PanasonicMnHeader::write