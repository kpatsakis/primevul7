    //! Convert C-string to upper case.
    inline void uppercase(char *const str) {
      if (str) for (char *ptr = str; *ptr; ++ptr) *ptr = uppercase(*ptr);