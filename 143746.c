    **/
    const CImg<T>& save_pnm(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      return _save_pnm(0,filename,bytes_per_pixel);