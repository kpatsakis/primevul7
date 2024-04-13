    DataBuf decodeAi7Thumbnail(const DataBuf &src)
    {
        const byte *colorTable = src.pData_;
        const long colorTableSize = 256 * 3;
        if (src.size_ < colorTableSize) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Invalid size of AI7 thumbnail: " << src.size_ << "\n";
#endif
            return DataBuf();
        }
        const byte *imageData = src.pData_ + colorTableSize;
        const long imageDataSize = (uint32_t)src.size_ - colorTableSize;
        const bool rle = (imageDataSize >= 3 && imageData[0] == 'R' && imageData[1] == 'L' && imageData[2] == 'E');
        std::string dest;
        for (long i = rle ? 3 : 0; i < imageDataSize;) {
            byte num = 1;
            byte value = imageData[i++];
            if (rle && value == 0xFD) {
                if (i >= imageDataSize) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Unexpected end of image data at AI7 thumbnail.\n";
#endif
                    return DataBuf();
                }
                value = imageData[i++];
                if (value != 0xFD) {
                    if (i >= imageDataSize) {
#ifndef SUPPRESS_WARNINGS
                        EXV_WARNING << "Unexpected end of image data at AI7 thumbnail.\n";
#endif
                        return DataBuf();
                    }
                    num = value;
                    value = imageData[i++];
                }
            }
            for (; num != 0; num--) {
                dest.append(reinterpret_cast<const char*>(colorTable + (3*value)), 3);
            }
        }
        return DataBuf(reinterpret_cast<const byte*>(dest.data()), static_cast<long>(dest.size()));
    }