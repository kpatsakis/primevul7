    void CrwMap::encode0x080a(const Image&      image,
                              const CrwMapping* pCrwMapping,
                                    CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        const ExifKey k1("Exif.Image.Make");
        const ExifKey k2("Exif.Image.Model");
        const ExifData::const_iterator ed1 = image.exifData().findKey(k1);
        const ExifData::const_iterator ed2 = image.exifData().findKey(k2);
        const ExifData::const_iterator edEnd = image.exifData().end();

        size_t size = 0;
        if (ed1 != edEnd) size += ed1->size();
        if (ed2 != edEnd) size += ed2->size();
        if (size != 0) {
            DataBuf buf(size);
            if (ed1 != edEnd) ed1->copy(buf.pData_, pHead->byteOrder());
            if (ed2 != edEnd) ed2->copy(buf.pData_ + ed1->size(), pHead->byteOrder());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encode0x080a