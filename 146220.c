    uint32_t TiffImageEntry::doWriteImage(IoWrapper& ioWrapper,
                                          ByteOrder  /*byteOrder*/) const
    {
        if ( !pValue() ) throw Error(kerImageWriteFailed); // #1296

        uint32_t len = pValue()->sizeDataArea();
        if (len > 0) {
#ifdef DEBUG
            std::cerr << "TiffImageEntry, Directory " << groupName(group())
                      << ", entry 0x" << std::setw(4)
                      << std::setfill('0') << std::hex << tag() << std::dec
                      << ": Writing data area, size = " << len;
#endif
            DataBuf buf = pValue()->dataArea();
            ioWrapper.write(buf.pData_, buf.size_);
            uint32_t align = len & 1;       // Align image data to word boundary
            if (align) ioWrapper.putb(0x0);
            len += align;
        }
        else {
#ifdef DEBUG
            std::cerr << "TiffImageEntry, Directory " << groupName(group())
                      << ", entry 0x" << std::setw(4)
                      << std::setfill('0') << std::hex << tag() << std::dec
                      << ": Writing " << strips_.size() << " strips";
#endif
            len = 0;
            for (Strips::const_iterator i = strips_.begin(); i != strips_.end(); ++i) {
                ioWrapper.write(i->first, i->second);
                len += i->second;
                uint32_t align = i->second & 1; // Align strip data to word boundary
                if (align) ioWrapper.putb(0x0);
                len += align;
            }
        }
#ifdef DEBUG
        std::cerr << ", len = " << len << " bytes\n";
#endif
        return len;
    } // TiffImageEntry::doWriteImage