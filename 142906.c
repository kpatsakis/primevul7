    //! Convert pixel values from HSL to RGB color spaces \newinstance.
    CImg<Tuchar> get_HSLtoRGB() const {
      return CImg<Tuchar>(*this,false).HSLtoRGB();