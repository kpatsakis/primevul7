    //! Convert pixel values from RGB to xyY color spaces \newinstance.
    CImg<Tfloat> get_RGBtoxyY(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoxyY(use_D65);