    long PreviewImage::writeFile(const std::string& path) const
    {
        std::string name = path + extension();
        // Todo: Creating a DataBuf here unnecessarily copies the memory
        DataBuf buf(pData_, size_);
        return Exiv2::writeFile(buf, name);
    }