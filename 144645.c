    //! Load image from a BMP file \newinstance.
    static CImg<T> get_load_bmp(std::FILE *const file) {
      return CImg<T>().load_bmp(file);