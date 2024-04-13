    //! Version of 'fseek()' that supports >=64bits offsets everywhere (for Windows).
    inline int fseek(FILE *stream, cimg_long offset, int origin) {
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      return _fseeki64(stream,(__int64)offset,origin);
#else
      return std::fseek(stream,offset,origin);
#endif