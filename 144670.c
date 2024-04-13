    //! Convert pixel values from HSI to RGB color spaces \newinstance.
    CImg<Tfloat> get_HSItoRGB() const {
      return CImg< Tuchar>(*this,false).HSItoRGB();