    //! Load image from a PANDORE-5 file \overloading.
    CImg<T>& load_pandore(std::FILE *const file) {
      return _load_pandore(file,0);