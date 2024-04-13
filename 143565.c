    **/
    CImg<T>& load_png(const char *const filename, unsigned int *const bits_per_pixel=0) {
      return _load_png(0,filename,bits_per_pixel);