    //! Convert pixel values from RGB to CMYK color spaces.
    CImg<T>& RGBtoCMYK() {
      return RGBtoCMY().CMYtoCMYK();