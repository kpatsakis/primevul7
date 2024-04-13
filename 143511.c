    //! Convert pixel values from RGB to sRGB color spaces \newinstance.
    CImg<Tfloat> get_RGBtosRGB() const {
      return CImg<Tfloat>(*this,false).RGBtosRGB();