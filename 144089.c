    **/
    CImg<T>& load_rgb(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgb(0,filename,dimw,dimh);