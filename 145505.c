    int dontOverwrite(const std::string& path)
    {
        if (path == "-")
            return 0;

        if (!Params::instance().force_ && Exiv2::fileExists(path)) {
            std::cout << Params::instance().progname() << ": " << _("Overwrite") << " `" << path << "'? ";
            std::string s;
            std::cin >> s;
            if (s[0] != 'y' && s[0] != 'Y')
                return 1;
        }
        return 0;
    }