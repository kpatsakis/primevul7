    **/
    static CImg<T> string(const char *const str, const bool is_last_zero=true, const bool is_shared=false) {
      if (!str) return CImg<T>();
      return CImg<T>(str,(unsigned int)std::strlen(str) + (is_last_zero?1:0),1,1,1,is_shared);