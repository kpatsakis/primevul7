    int Insert::insertXmpPacket(const std::string& path, const Exiv2::DataBuf& xmpBlob, bool usePacket) const
    {
        std::string xmpPacket;
        for (size_t i = 0; i < xmpBlob.size_; i++) {
            xmpPacket += static_cast<char>(xmpBlob.pData_[i]);
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
        image->readMetadata();
        image->clearXmpData();
        image->setXmpPacket(xmpPacket);
        image->writeXmpFromPacket(usePacket);
        image->writeMetadata();

        return 0;
    }