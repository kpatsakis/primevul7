    void CrwParser::encode(
              Blob&     blob,
        const byte*     pData,
              uint32_t  size,
        const CrwImage* pCrwImage
    )
    {
        // Parse image, starting with a CIFF header component
        CiffHeader::UniquePtr head(new CiffHeader);
        if (size != 0) {
            head->read(pData, size);
        }

        // Encode Exif tags from image into the CRW parse tree and write the
        // structure to the binary image blob
        CrwMap::encode(head.get(), *pCrwImage);
        head->write(blob);

    } // CrwParser::encode