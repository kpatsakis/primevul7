    **/
    CImg<T>& mirror(const char *const axes) {
      for (const char *s = axes; *s; ++s) mirror(*s);
      return *this;