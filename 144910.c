    //! Convert pixel values from RGB to XYZ color spaces \newinstance.
    CImg<Tfloat> get_RGBtoXYZ(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoXYZ(use_D65);