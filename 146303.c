    uint32_t TiffEncoder::updateDirEntry(byte* buf,
                                         ByteOrder byteOrder,
                                         TiffComponent* pTiffComponent) const
    {
        assert(buf);
        assert(pTiffComponent);
        TiffEntryBase* pTiffEntry = dynamic_cast<TiffEntryBase*>(pTiffComponent);
        assert(pTiffEntry);
        us2Data(buf + 2, pTiffEntry->tiffType(), byteOrder);
        ul2Data(buf + 4, pTiffEntry->count(),    byteOrder);
        // Move data to offset field, if it fits and is not yet there.
        if (pTiffEntry->size() <= 4 && buf + 8 != pTiffEntry->pData()) {
#ifdef DEBUG
            std::cerr << "Copying data for tag " << pTiffEntry->tag()
                      << " to offset area.\n";
#endif
            memset(buf + 8, 0x0, 4);
            memcpy(buf + 8, pTiffEntry->pData(), pTiffEntry->size());
            memset(const_cast<byte*>(pTiffEntry->pData()), 0x0, pTiffEntry->size());
        }
        return 12;
    }