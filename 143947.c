    **/
    const CImg<T>& save_bmp(const char *const filename) const {
      return _save_bmp(0,filename);