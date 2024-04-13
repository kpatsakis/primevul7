    const char* exvGettext(const char* str)
    {
#ifdef EXV_ENABLE_NLS
        return _exvGettext(str);
#else
        return str;
#endif
    }