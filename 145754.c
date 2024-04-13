    bool convertStringCharsetIconv(std::string& str, const char* from, const char* to)
    {
        if (0 == strcmp(from, to)) return true; // nothing to do

        bool ret = true;
        iconv_t cd;
        cd = iconv_open(to, from);
        if (cd == (iconv_t)(-1)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "iconv_open: " << strError() << "\n";
#endif
            return false;
        }
        std::string outstr;
        EXV_ICONV_CONST char* inptr = const_cast<char*>(str.c_str());
        size_t inbytesleft = str.length();
        while (inbytesleft) {
            char outbuf[256];
            char* outptr = outbuf;
            size_t outbytesleft = sizeof(outbuf);
            size_t rc = iconv(cd,
                              &inptr,
                              &inbytesleft,
                              &outptr,
                              &outbytesleft);
            const size_t outbytesProduced = sizeof(outbuf) - outbytesleft;
            if (rc == size_t(-1) && errno != E2BIG) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "iconv: " << strError()
                            << " inbytesleft = " << inbytesleft << "\n";
#endif
                ret = false;
                break;
            }
            outstr.append(std::string(outbuf, outbytesProduced));
        }
        if (cd != (iconv_t)(-1)) {
            iconv_close(cd);
        }

        if (ret) str = outstr;
        return ret;
    }