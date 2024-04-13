    DataBuf nikonCrypt(uint16_t tag, const byte* pData, uint32_t size, TiffComponent* const pRoot)
    {
        DataBuf buf;

        if (size < 4) return buf;
        const NikonArrayIdx* nci = find(nikonArrayIdx, NikonArrayIdx::Key(tag, reinterpret_cast<const char*>(pData), size));
        if (nci == 0 || nci->start_ == NA || size <= nci->start_) return buf;

        // Find Exif.Nikon3.ShutterCount
        TiffFinder finder(0x00a7, nikon3Id);
        pRoot->accept(finder);
        TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
        if (!te || !te->pValue() || te->pValue()->count() == 0) return buf;
        uint32_t count = static_cast<uint32_t>(te->pValue()->toLong());

        // Find Exif.Nikon3.SerialNumber
        finder.init(0x001d, nikon3Id);
        pRoot->accept(finder);
        te = dynamic_cast<TiffEntryBase*>(finder.result());
        if (!te || !te->pValue() || te->pValue()->count() == 0) return buf;
        bool ok(false);
        uint32_t serial = stringTo<uint32_t>(te->pValue()->toString(), ok);
        if (!ok) {
            std::string model = getExifModel(pRoot);
            if (model.empty()) return buf;
            if (model.find("D50") != std::string::npos) {
                serial = 0x22;
            }
            else {
                serial = 0x60;
            }
        }
        buf.alloc(size);
        memcpy(buf.pData_, pData, buf.size_);
        ncrypt(buf.pData_ + nci->start_, buf.size_ - nci->start_, count, serial);
        return buf;
    }