    //! Load image from a PNM file \overloading.
    CImg<T>& load_pnm(std::FILE *const file) {
      return _load_pnm(file,0);