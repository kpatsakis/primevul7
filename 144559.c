    //! Mirror image content along specified axes \newinstance.
    CImg<T> get_mirror(const char *const axes) const {
      return (+*this).mirror(axes);