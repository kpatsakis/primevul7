    uint32_t TiffDirectory::writeDirEntry(IoWrapper&     ioWrapper,
                                          ByteOrder      byteOrder,
                                          int32_t        offset,
                                          TiffComponent* pTiffComponent,
                                          uint32_t       valueIdx,
                                          uint32_t       dataIdx,
                                          uint32_t&      imageIdx) const
    {
        assert(pTiffComponent);
        TiffEntryBase* pDirEntry = dynamic_cast<TiffEntryBase*>(pTiffComponent);
        assert(pDirEntry);
        byte buf[8];
        us2Data(buf,     pDirEntry->tag(),      byteOrder);
        us2Data(buf + 2, pDirEntry->tiffType(), byteOrder);
        ul2Data(buf + 4, pDirEntry->count(),    byteOrder);
        ioWrapper.write(buf, 8);
        if (pDirEntry->size() > 4) {
            pDirEntry->setOffset(offset + static_cast<int32_t>(valueIdx));
            l2Data(buf, pDirEntry->offset(), byteOrder);
            ioWrapper.write(buf, 4);
        }
        else {
            const uint32_t len = pDirEntry->write(ioWrapper,
                                                  byteOrder,
                                                  offset,
                                                  valueIdx,
                                                  dataIdx,
                                                  imageIdx);
            assert(len <= 4);
            if (len < 4) {
                memset(buf, 0x0, 4);
                ioWrapper.write(buf, 4 - len);
            }
        }
        return 12;
    } // TiffDirectory::writeDirEntry