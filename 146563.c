    SigmaMnHeader::SigmaMnHeader(): start_(0)
    {
        read(signature1_, sizeOfSignature(), invalidByteOrder);
    }