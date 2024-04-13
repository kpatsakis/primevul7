    //! Load image from a PFM file \newinstance.
    static CImg<T> get_load_pfm(std::FILE *const file) {
      return CImg<T>().load_pfm(file);