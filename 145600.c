    uint32_t TiffEntryBase::writeOffset(byte*     buf,
                                        int32_t   offset,
                                        TiffType  tiffType,
                                        ByteOrder byteOrder)
    {
        uint32_t rc = 0;
        switch(tiffType) {
        case ttUnsignedShort:
        case ttSignedShort:
            if (static_cast<uint32_t>(offset) > 0xffff) throw Error(kerOffsetOutOfRange);
            rc = s2Data(buf, static_cast<int16_t>(offset), byteOrder);
            break;
        case ttUnsignedLong:
        case ttSignedLong:
            rc = l2Data(buf, static_cast<int32_t>(offset), byteOrder);
            break;
        default:
            throw Error(kerUnsupportedDataAreaOffsetType);
            break;
        }
        return rc;
    } // TiffEntryBase::writeOffset