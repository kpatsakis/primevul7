    //! Convert pixel values from RGB to HSL color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSL() const {
      return CImg<Tfloat>(*this,false).RGBtoHSL();