    //! Load image from a PNG file \newinstance.
    static CImg<T> get_load_png(const char *const filename, unsigned int *const bits_per_pixel=0) {
      return CImg<T>().load_png(filename,bits_per_pixel);