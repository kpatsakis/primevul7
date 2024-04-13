    //! Load image from a RGBA file \newinstance.
    static CImg<T> get_load_rgba(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgba(file,dimw,dimh);