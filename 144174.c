    **/
    CImg<T>& cumulate(const char *const axes) {
      for (const char *s = axes; *s; ++s) cumulate(*s);
      return *this;