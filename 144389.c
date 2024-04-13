    //! Convert pixel values from RGB to HSV color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSV() const {
      return CImg<Tfloat>(*this,false).RGBtoHSV();