    //! Load gzipped image file, using external tool 'gunzip' \newinstance.
    static CImg<T> get_load_gzip_external(const char *const filename) {
      return CImg<T>().load_gzip_external(filename);