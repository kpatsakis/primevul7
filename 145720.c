    PentaxMnHeader::PentaxMnHeader()
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }