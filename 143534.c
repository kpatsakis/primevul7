    //! Load image from a BMP file \newinstance.
    static CImg<T> get_load_bmp(const char *const filename) {
      return CImg<T>().load_bmp(filename);