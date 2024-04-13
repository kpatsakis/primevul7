    uint32_t TiffImageEntry::doWrite(IoWrapper& ioWrapper,
                                     ByteOrder byteOrder,
                                     int32_t   offset,
                                     uint32_t  /*valueIdx*/,
                                     uint32_t  dataIdx,
                                     uint32_t& imageIdx)
    {
        uint32_t o2 = imageIdx;
        // For makernotes, write TIFF image data to the data area
        if (group() > mnId) o2 = offset + dataIdx;
#ifdef DEBUG
        std::cerr << "TiffImageEntry, Directory " << groupName(group())
                  << ", entry 0x" << std::setw(4)
                  << std::setfill('0') << std::hex << tag() << std::dec
                  << ": Writing offset " << o2 << "\n";
#endif
        DataBuf buf(static_cast<long>(strips_.size()) * 4);
        memset(buf.pData_, 0x0, buf.size_);
        uint32_t idx = 0;
        for (Strips::const_iterator i = strips_.begin(); i != strips_.end(); ++i) {
            idx += writeOffset(buf.pData_ + idx, o2, tiffType(), byteOrder);
            o2 += i->second;
            o2 += i->second & 1;                // Align strip data to word boundary
            if (!(group() > mnId)) {            // Todo: FIX THIS!! SHOULDN'T USE >
                imageIdx += i->second;
                imageIdx += i->second & 1;      // Align strip data to word boundary
            }
        }
        ioWrapper.write(buf.pData_, buf.size_);
        return (uint32_t)buf.size_;
    } // TiffImageEntry::doWrite