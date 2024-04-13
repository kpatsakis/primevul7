    uint32_t TiffImageEntry::doSizeData() const
    {
        uint32_t len = 0;
        // For makernotes, TIFF image data is written to the data area
        if (group() > mnId) { // Todo: Fix this hack!!
            len = sizeImage();
        }
        return len;
    } // TiffImageEntry::doSizeData