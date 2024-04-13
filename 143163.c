    //! Load image from a file, using Magick++ library \newinstance.
    static CImg<T> get_load_magick(const char *const filename) {
      return CImg<T>().load_magick(filename);