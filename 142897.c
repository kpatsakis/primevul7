    //! Fill image with random values in specified range \newinstance.
    CImg<T> get_rand(const T& val_min, const T& val_max) const {
      return (+*this).rand(val_min,val_max);