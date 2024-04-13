    int renameFile(std::string& newPath, const struct tm* tm)
    {
        std::string path = newPath;
        std::string format = Params::instance().format_;
        Util::replace(format, ":basename:", Util::basename(path, true));
        Util::replace(format, ":dirname:", Util::basename(Util::dirname(path)));
        Util::replace(format, ":parentname:", Util::basename(Util::dirname(Util::dirname(path))));

        const size_t max = 1024;
        char basename[max];
        std::memset(basename, 0x0, max);
        if (strftime(basename, max, format.c_str(), tm) == 0) {
            std::cerr << _("Filename format yields empty filename for the file") << " " << path << "\n";
            return 1;
        }
        newPath = Util::dirname(path) + EXV_SEPARATOR_STR + basename + Util::suffix(path);
        if (Util::dirname(newPath) == Util::dirname(path) && Util::basename(newPath) == Util::basename(path)) {
            if (Params::instance().verbose_) {
                std::cout << _("This file already has the correct name") << std::endl;
            }
            return -1;
        }

        bool go = true;
        int seq = 1;
        std::string s;
        Params::FileExistsPolicy fileExistsPolicy = Params::instance().fileExistsPolicy_;
        while (go) {
            if (Exiv2::fileExists(newPath)) {
                switch (fileExistsPolicy) {
                    case Params::overwritePolicy:
                        go = false;
                        break;
                    case Params::renamePolicy:
                        newPath = Util::dirname(path) + EXV_SEPARATOR_STR + basename + "_" + Exiv2::toString(seq++) +
                                  Util::suffix(path);
                        break;
                    case Params::askPolicy:
                        std::cout << Params::instance().progname() << ": " << _("File") << " `" << newPath << "' "
                                  << _("exists. [O]verwrite, [r]ename or [s]kip?") << " ";
                        std::cin >> s;
                        switch (s[0]) {
                            case 'o':
                            case 'O':
                                go = false;
                                break;
                            case 'r':
                            case 'R':
                                fileExistsPolicy = Params::renamePolicy;
                                newPath = Util::dirname(path) + EXV_SEPARATOR_STR + basename + "_" +
                                          Exiv2::toString(seq++) + Util::suffix(path);
                                break;
                            default:  // skip
                                return -1;
                        }
                }
            } else {
                go = false;
            }
        }

        if (Params::instance().verbose_) {
            std::cout << _("Renaming file to") << " " << newPath;
            if (Params::instance().timestamp_) {
                std::cout << ", " << _("updating timestamp");
            }
            std::cout << std::endl;
        }

        // Workaround for MinGW rename which does not overwrite existing files
        remove(newPath.c_str());
        if (std::rename(path.c_str(), newPath.c_str()) == -1) {
            std::cerr << Params::instance().progname() << ": " << _("Failed to rename") << " " << path << " " << _("to")
                      << " " << newPath << ": " << Exiv2::strError() << "\n";
            return 1;
        }

        return 0;
    }