    //! Save image as a JPEG file \overloading.
    const CImg<T>& save_jpeg(std::FILE *const file, const unsigned int quality=100) const {
      return _save_jpeg(file,0,quality);