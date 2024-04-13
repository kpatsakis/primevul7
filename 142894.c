    //! Load image from a PNG file \newinstance.
    static CImg<T> get_load_png(std::FILE *const file, unsigned int *const bits_per_pixel=0) {
      return CImg<T>().load_png(file,bits_per_pixel);