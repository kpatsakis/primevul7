    int Timestamp::touch(const std::string& path)
    {
        if (0 == actime_)
            return 1;
        struct utimbuf buf;
        buf.actime = actime_;
        buf.modtime = modtime_;
        return utime(path.c_str(), &buf);
    }