    //! Convert pixel values from CMYK to RGB color spaces.
    CImg<T>& CMYKtoRGB() {
      return CMYKtoCMY().CMYtoRGB();