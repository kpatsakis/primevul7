    //! Return the basename of a filename.
    inline const char* basename(const char *const s, const char separator=cimg_file_separator)  {
      const char *p = 0, *np = s;
      while (np>=s && (p=np)) np = std::strchr(np,separator) + 1;
      return p;