    //! Save image as a BMP file \overloading.
    const CImg<T>& save_bmp(std::FILE *const file) const {
      return _save_bmp(file,0);