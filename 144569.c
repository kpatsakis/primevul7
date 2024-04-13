    //! Version of 'ftell()' that supports >=64bits offsets everywhere (for Windows).
    inline cimg_long ftell(FILE *stream) {
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      return (cimg_long)_ftelli64(stream);
#else
      return (cimg_long)std::ftell(stream);
#endif