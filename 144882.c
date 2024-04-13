    //! Convert pixel values from CMY to CMYK color spaces.
    CImg<T>& CMYtoCMYK() {
      return get_CMYtoCMYK().move_to(*this);