    //! Mirror image content along specified axis \newinstance.
    CImg<T> get_mirror(const char axis) const {
      return (+*this).mirror(axis);