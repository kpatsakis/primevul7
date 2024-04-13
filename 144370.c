    //! Convert pixel values from RGB to Lab color spaces \newinstance.
    CImg<Tfloat> get_RGBtoLab(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoLab(use_D65);