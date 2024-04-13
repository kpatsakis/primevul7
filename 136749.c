    void CiffDirectory::doRead(const byte* pData,
                               uint32_t    size,
                               uint32_t    start,
                               ByteOrder   byteOrder)
    {
        CiffComponent::doRead(pData, size, start, byteOrder);
#ifdef DEBUG
        std::cout << "Reading directory 0x" << std::hex << tag() << "\n";
#endif
        readDirectory(pData + offset(), this->size(), byteOrder);
#ifdef DEBUG
        std::cout << "<---- 0x" << std::hex << tag() << "\n";
#endif
    } // CiffDirectory::doRead