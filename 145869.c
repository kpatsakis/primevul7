    Nikon2MnHeader::Nikon2MnHeader() : start_(0)
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }