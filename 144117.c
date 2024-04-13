    // Convert filename as a Windows-style filename (short path name).
    inline void winformat_string(char *const str) {
      if (str && *str) {
#if cimg_OS==2
        char *const nstr = new char[MAX_PATH];
        if (GetShortPathNameA(str,nstr,MAX_PATH)) std::strcpy(str,nstr);
        delete[] nstr;
#endif
      }