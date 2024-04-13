    **/
    const CImg<T>& save_ascii(const char *const filename) const {
      return _save_ascii(0,filename);