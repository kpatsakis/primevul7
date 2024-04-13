    //! Convert pixel values from HSV to RGB color spaces \newinstance.
    CImg<Tuchar> get_HSVtoRGB() const {
      return CImg<Tuchar>(*this,false).HSVtoRGB();