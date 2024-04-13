    **/
    CImg<T>& load_rgba(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgba(0,filename,dimw,dimh);