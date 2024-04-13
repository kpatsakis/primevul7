    //! Autocrop image region, regarding the specified background value \newinstance.
    CImg<T> get_autocrop(const T& value, const char *const axes="czyx") const {
      return (+*this).autocrop(value,axes);