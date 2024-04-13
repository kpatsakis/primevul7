    //! Convert pixel values from CMYK to RGB color spaces \newinstance.
    CImg<Tuchar> get_CMYKtoRGB() const {
      return CImg<Tuchar>(*this,false).CMYKtoRGB();