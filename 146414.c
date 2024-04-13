    void TiffDataEntry::setStrips(const Value* pSize,
                                  const byte*  pData,
                                  uint32_t     sizeData,
                                  uint32_t     baseOffset)
    {
        if (!pValue() || !pSize) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << ": Size or data offset value not set, ignoring them.\n";
#endif
            return;
        }
        if (pValue()->count() == 0) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << ": Data offset entry value is empty, ignoring it.\n";
#endif
            return;
        }
        if (pValue()->count() != pSize->count()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << ": Size and data offset entries have different"
                        << " number of components, ignoring them.\n";
#endif
            return;
        }
        uint32_t size = 0;
        for (int i = 0; i < pSize->count(); ++i) {
            size += static_cast<uint32_t>(pSize->toLong(i));
        }
        uint32_t offset = static_cast<uint32_t>(pValue()->toLong(0));
        // Todo: Remove limitation of JPEG writer: strips must be contiguous
        // Until then we check: last offset + last size - first offset == size?
        if (  static_cast<uint32_t>(pValue()->toLong(pValue()->count()-1))
            + static_cast<uint32_t>(pSize->toLong(pSize->count()-1))
            - offset != size) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << ": Data area is not contiguous, ignoring it.\n";
#endif
            return;
        }
        if (   offset > sizeData
            || size > sizeData
            || baseOffset + offset > sizeData - size) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << ": Data area exceeds data buffer, ignoring it.\n";
#endif
            return;
        }
        pDataArea_ = const_cast<byte*>(pData) + baseOffset + offset;
        sizeDataArea_ = size;
        const_cast<Value*>(pValue())->setDataArea(pDataArea_, sizeDataArea_);
    } // TiffDataEntry::setStrips