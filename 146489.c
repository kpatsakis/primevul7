    std::string newFilePath(const std::string& path, const std::string& ext)
    {
        std::string directory = Params::instance().directory_;
        if (directory.empty())
            directory = Util::dirname(path);
        directory = Exiv2::fileProtocol(path) == Exiv2::pFile ? directory + EXV_SEPARATOR_STR
                                                              : ""  // use current directory for remote files
            ;
        return directory + Util::basename(path, true) + ext;
    }