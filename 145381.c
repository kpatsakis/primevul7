    long PreviewImage::writeFile(const std::wstring& wpath) const
    {
        std::wstring name = wpath + wextension();
        // Todo: Creating a DataBuf here unnecessarily copies the memory
        DataBuf buf(pData_, size_);
        return Exiv2::writeFile(buf, name);
    }