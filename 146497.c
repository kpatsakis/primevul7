    void TiffImageEntry::setStrips(const Value* pSize,
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
        for (int i = 0; i < pValue()->count(); ++i) {
            const uint32_t offset = static_cast<uint32_t>(pValue()->toLong(i));
            const byte* pStrip = pData + baseOffset + offset;
            const uint32_t size = static_cast<uint32_t>(pSize->toLong(i));

            if (   offset > sizeData
                || size > sizeData
                || baseOffset + offset > sizeData - size) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Directory " << groupName(group())
                            << ", entry 0x" << std::setw(4)
                            << std::setfill('0') << std::hex << tag()
                            << ": Strip " << std::dec << i
                            << " is outside of the data area; ignored.\n";
#endif
            }
            else if (size != 0) {
                strips_.push_back(std::make_pair(pStrip, size));
            }
        }
    } // TiffImageEntry::setStrips