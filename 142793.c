    //! Convert pixel values from sRGB to RGB color spaces \newinstance.
    CImg<Tfloat> get_sRGBtoRGB() const {
      return CImg<Tfloat>(*this,false).sRGBtoRGB();