    //! Convert C-string to lower case.
    inline void lowercase(char *const str) {
      if (str) for (char *ptr = str; *ptr; ++ptr) *ptr = lowercase(*ptr);