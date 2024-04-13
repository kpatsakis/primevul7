    //! Convert pixel values from CMY to RGB color spaces \newinstance.
    CImg<Tuchar> get_CMYtoRGB() const {
      return CImg<Tuchar>(*this,false).CMYtoRGB();