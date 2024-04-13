    //! Load image from a RGBA file \overloading.
    CImg<T>& load_rgba(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgba(file,0,dimw,dimh);