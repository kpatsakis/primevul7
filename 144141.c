    //! Load image from a PNG file \overloading.
    CImg<T>& load_png(std::FILE *const file, unsigned int *const bits_per_pixel=0) {
      return _load_png(file,0,bits_per_pixel);