    **/
    const CImg<T>& save_pfm(const char *const filename) const {
      get_mirror('y')._save_pfm(0,filename);
      return *this;