    //! Load image using ImageMagick's external tool 'convert' \newinstance.
    static CImg<T> get_load_imagemagick_external(const char *const filename) {
      return CImg<T>().load_imagemagick_external(filename);