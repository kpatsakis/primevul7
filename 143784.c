    //! Load image from a BMP file \overloading.
    CImg<T>& load_bmp(std::FILE *const file) {
      return _load_bmp(file,0);