    //! Load image from a PFM file \overloading.
    CImg<T>& load_pfm(std::FILE *const file) {
      return _load_pfm(file,0);