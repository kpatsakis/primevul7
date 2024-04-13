    //! Load image from a RAW Color Camera file, using external tool 'dcraw' \newinstance.
    static CImg<T> get_load_dcraw_external(const char *const filename) {
      return CImg<T>().load_dcraw_external(filename);