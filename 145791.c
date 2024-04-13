        std::string getExiv2ConfigPath()
        {
            std::string homedir;
            std::string inifile;
#if defined(_MSC_VER) || defined(__MINGW__)
            char path[MAX_PATH];
            if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path))) {
                homedir = std::string(path);
                inifile = "exiv2.ini"      ;
            }
#else
            struct passwd* pw = getpwuid(getuid());
            homedir = std::string(pw?pw->pw_dir:"");
            inifile = std::string(".exiv2");
#endif
            return homedir + EXV_SEPARATOR_CHR + inifile;
        }