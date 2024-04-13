    DataBuf makePnm(uint32_t width, uint32_t height, const DataBuf &rgb)
    {
        const size_t expectedSize = static_cast<size_t>(width * height * 3);
        if (rgb.size_ != expectedSize) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Invalid size of preview data. Expected " << expectedSize << " bytes, got " << rgb.size_ << " bytes.\n";
#endif
            return DataBuf();
        }

        const std::string header = "P6\n" + toString(width) + " " + toString(height) + "\n255\n";
        const byte *headerBytes = reinterpret_cast<const byte*>(header.data());

        DataBuf dest(static_cast<long>(header.size() + rgb.size_));
        std::copy(headerBytes, headerBytes + header.size(), dest.pData_);
        std::copy(rgb.pData_, rgb.pData_ + rgb.size_, dest.pData_ + header.size());
        return dest;
    }