    OlympusMnHeader::OlympusMnHeader()
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }