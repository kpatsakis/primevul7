    //! Convert pixel values from CMYK to CMY color spaces.
    CImg<T>& CMYKtoCMY() {
      return get_CMYKtoCMY().move_to(*this);