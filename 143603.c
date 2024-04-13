    //! Fill image with a linear sequence of values \newinstance.
    CImg<T> get_sequence(const T& a0, const T& a1) const {
      return (+*this).sequence(a0,a1);