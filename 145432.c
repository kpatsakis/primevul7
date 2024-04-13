    Casio2MnHeader::Casio2MnHeader(): start_(0)
    {
        read(signature_, sizeOfSignature(), invalidByteOrder );
    }