    PentaxDngMnHeader::PentaxDngMnHeader()
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }