    //! Pointwise min operator between instance image and a value \newinstance.
    CImg<T> get_min(const T& value) const {
      return (+*this).min(value);