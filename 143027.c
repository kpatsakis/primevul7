    //! Convert pixel values from RGB to HSI color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSI() const {
      return CImg<Tfloat>(*this,false).RGBtoHSI();