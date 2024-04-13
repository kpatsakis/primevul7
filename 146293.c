    SonyMnHeader::SonyMnHeader(): start_(0)
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }