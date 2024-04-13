    **/
    const CImg<T>& save_pandore(const char *const filename, const unsigned int colorspace=0) const {
      return _save_pandore(0,filename,colorspace);