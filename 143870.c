    //! Load image from a PANDORE-5 file \newinstance.
    static CImg<T> get_load_pandore(const char *const filename) {
      return CImg<T>().load_pandore(filename);