    FujiMnHeader::FujiMnHeader() : start_(0)
    {
        read(signature_, sizeOfSignature(), byteOrder_);
    }