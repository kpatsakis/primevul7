    uint32_t TiffDirectory::doWrite(IoWrapper& ioWrapper,
                                    ByteOrder byteOrder,
                                    int32_t   offset,
                                    uint32_t  valueIdx,
                                    uint32_t  dataIdx,
                                    uint32_t& imageIdx)
    {
        bool isRootDir = (imageIdx == uint32_t(-1));

        // Number of components to write
        const uint32_t compCount = count();
        if (compCount > 0xffff) throw Error(kerTooManyTiffDirectoryEntries, groupName(group()));

        // Size of next IFD, if any
        uint32_t sizeNext = 0;
        if (pNext_) sizeNext = pNext_->size();

        // Nothing to do if there are no entries and the size of the next IFD is 0
        if (compCount == 0 && sizeNext == 0) return 0;

        // Remember the offset of the CR2 RAW IFD
        if (group() == ifd3Id) {
#ifdef DEBUG
            std::cerr << "Directory " << groupName(group()) << " offset is 0x"
                      << std::setw(8) << std::setfill('0') << std::hex << offset << std::dec
                      << "\n";
#endif
            ioWrapper.setTarget(OffsetWriter::cr2RawIfdOffset, offset);
        }
        // Size of all directory entries, without values and additional data
        const uint32_t sizeDir = 2 + 12 * compCount + (hasNext_ ? 4 : 0);

        // TIFF standard requires IFD entries to be sorted in ascending order by tag.
        // Not sorting makernote directories sometimes preserves them better.
        if (group() < mnId) {
            std::sort(components_.begin(), components_.end(), cmpTagLt);
        }
        // Size of IFD values and additional data
        uint32_t sizeValue = 0;
        uint32_t sizeData = 0;
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            uint32_t sv = (*i)->size();
            if (sv > 4) {
                sv += sv & 1;               // Align value to word boundary
                sizeValue += sv;
            }
            // Also add the size of data, but only if needed
            if (isRootDir) {
                uint32_t sd = (*i)->sizeData();
                sd += sd & 1;               // Align data to word boundary
                sizeData += sd;
            }
        }

        uint32_t idx = 0;                   // Current IFD index / bytes written
        valueIdx = sizeDir;                 // Offset to the current IFD value
        dataIdx  = sizeDir + sizeValue;     // Offset to the entry's data area
        if (isRootDir) {                    // Absolute offset to the image data
            imageIdx = offset + dataIdx + sizeData + sizeNext;
            imageIdx += imageIdx & 1;       // Align image data to word boundary
        }

        // 1st: Write the IFD, a) Number of directory entries
        byte buf[4];
        us2Data(buf, static_cast<uint16_t>(compCount), byteOrder);
        ioWrapper.write(buf, 2);
        idx += 2;
        // b) Directory entries - may contain pointers to the value or data
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            idx += writeDirEntry(ioWrapper, byteOrder, offset, *i, valueIdx, dataIdx, imageIdx);
            uint32_t sv = (*i)->size();
            if (sv > 4) {
                sv += sv & 1;               // Align value to word boundary
                valueIdx += sv;
            }
            uint32_t sd = (*i)->sizeData();
            sd += sd & 1;                   // Align data to word boundary
            dataIdx += sd;
        }
        // c) Pointer to the next IFD
        if (hasNext_) {
            memset(buf, 0x0, 4);
            if (pNext_ && sizeNext) {
                l2Data(buf, offset + dataIdx, byteOrder);
            }
            ioWrapper.write(buf, 4);
            idx += 4;
        }
        assert(idx == sizeDir);

        // 2nd: Write IFD values - may contain pointers to additional data
        valueIdx = sizeDir;
        dataIdx = sizeDir + sizeValue;
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            uint32_t sv = (*i)->size();
            if (sv > 4) {
                uint32_t d = (*i)->write(ioWrapper, byteOrder, offset, valueIdx, dataIdx, imageIdx);
                enforce(sv == d, kerImageWriteFailed);
                if ((sv & 1) == 1) {
                    ioWrapper.putb(0x0);    // Align value to word boundary
                    sv += 1;
                }
                idx += sv;
                valueIdx += sv;
            }
            uint32_t sd = (*i)->sizeData();
            sd += sd & 1;                   // Align data to word boundary
            dataIdx += sd;
        }
        assert(idx == sizeDir + sizeValue);

        // 3rd: Write data - may contain offsets too (eg sub-IFD)
        dataIdx = sizeDir + sizeValue;
        idx += writeData(ioWrapper, byteOrder, offset, dataIdx, imageIdx);

        // 4th: Write next-IFD
        if (pNext_ && sizeNext) {
            idx += pNext_->write(ioWrapper, byteOrder, offset + idx, uint32_t(-1), uint32_t(-1), imageIdx);
        }

        // 5th, at the root directory level only: write image data
        if (isRootDir) {
            idx += writeImage(ioWrapper, byteOrder);
        }

        return idx;
    } // TiffDirectory::doWrite