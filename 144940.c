    **/
    const CImg<T>& save_png(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      return _save_png(0,filename,bytes_per_pixel);