    //! Load image from a RGB file \newinstance.
    static CImg<T> get_load_rgb(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgb(file,dimw,dimh);