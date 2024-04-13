    //! Load image from a PFM file \newinstance.
    static CImg<T> get_load_pfm(const char *const filename) {
      return CImg<T>().load_pfm(filename);