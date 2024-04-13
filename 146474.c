    constexpr uint32_t SigmaMnHeader::sizeOfSignature()
    {
        static_assert(sizeof(signature1_) == sizeof(signature2_), "Signatures must be of same size");
        return sizeof(signature1_);
    }