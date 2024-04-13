    int Timestamp::read(const std::string& path)
    {
        struct stat buf;
        int rc = stat(path.c_str(), &buf);
        if (0 == rc) {
            actime_ = buf.st_atime;
            modtime_ = buf.st_mtime;
        }
        return rc;
    }