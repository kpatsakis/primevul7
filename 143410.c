    **/
    const CImg<T>& save_jpeg(const char *const filename, const unsigned int quality=100) const {
      return _save_jpeg(0,filename,quality);