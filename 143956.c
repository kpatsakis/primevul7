    //! Pointwise max operator between instance image and a value \newinstance.
    CImg<T> get_max(const T& value) const {
      return (+*this).max(value);