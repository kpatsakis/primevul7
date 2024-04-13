    HttpIo::HttpImpl::HttpImpl(const std::string& url, size_t blockSize)
        : Impl(url, blockSize)
        , hostInfo_(Exiv2::Uri::Parse(url))
    {
        Exiv2::Uri::Decode(hostInfo_);
    }