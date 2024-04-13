    //! Load image from a PANDORE-5 file \newinstance.
    static CImg<T> get_load_pandore(std::FILE *const file) {
      return CImg<T>().load_pandore(file);