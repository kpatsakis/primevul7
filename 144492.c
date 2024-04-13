    //! Save image as a PNG file \overloading.
    const CImg<T>& save_png(std::FILE *const file, const unsigned int bytes_per_pixel=0) const {
      return _save_png(file,0,bytes_per_pixel);