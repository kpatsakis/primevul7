    **/
    const CImg<T>& save_rgb(const char *const filename) const {
      return _save_rgb(0,filename);