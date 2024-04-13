    //! Load image from a RGB file \overloading.
    CImg<T>& load_rgb(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgb(file,0,dimw,dimh);