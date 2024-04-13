    bool convertStringCharset(std::string &str, const char* from, const char* to)
    {
        if (0 == strcmp(from, to)) return true; // nothing to do
        bool ret = false;
#if defined EXV_HAVE_ICONV
        ret = convertStringCharsetIconv(str, from, to);
#elif defined WIN32 && !defined __CYGWIN__
        ret = convertStringCharsetWindows(str, from, to);
#else
# ifndef SUPPRESS_WARNINGS
        EXV_WARNING << "Charset conversion required but no character mapping functionality available.\n";
# endif
        UNUSED(str);
#endif
        return ret;
    }