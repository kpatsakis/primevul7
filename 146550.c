    void Image::clearXmpData()
    {
        xmpData_.clear();
        writeXmpFromPacket(false);
    }