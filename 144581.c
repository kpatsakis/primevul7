    //! Load image from an Ascii file \overloading.
    CImg<T>& load_ascii(std::FILE *const file) {
      return _load_ascii(file,0);