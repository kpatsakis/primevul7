    //! Load image from a JPEG file \overloading.
    CImg<T>& load_jpeg(std::FILE *const file) {
      return _load_jpeg(file,0);