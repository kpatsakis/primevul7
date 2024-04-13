    //! Autocrop image region, regarding the specified background color \newinstance.
    CImg<T> get_autocrop(const T *const color=0, const char *const axes="zyx") const {
      return (+*this).autocrop(color,axes);