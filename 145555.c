    uint32_t TiffBinaryArray::doCount() const
    {
        if (cfg() == 0 || !decoded()) return TiffEntryBase::doCount();

        if (elements_.empty()) return 0;

        TypeId typeId = toTypeId(tiffType(), tag(), group());
        long typeSize = TypeInfo::typeSize(typeId);
        if (0 == typeSize) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << tag()
                        << " has unknown Exif (TIFF) type " << std::dec << tiffType()
                        << "; setting type size 1.\n";
#endif
            typeSize = 1;
        }

        return static_cast<uint32_t>(static_cast<double>(size()) / typeSize + 0.5);
    }