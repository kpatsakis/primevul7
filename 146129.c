    PanasonicMnHeader::PanasonicMnHeader(): start_(0)
    {
        read(signature_, sizeOfSignature(), invalidByteOrder);
    }