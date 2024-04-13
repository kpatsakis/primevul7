    //! Load image from a PNM file \newinstance.
    static CImg<T> get_load_pnm(const char *const filename) {
      return CImg<T>().load_pnm(filename);