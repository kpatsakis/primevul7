    //! Convert pixel values from XYZ to Lab color spaces \newinstance.
    CImg<Tfloat> get_XYZtoLab(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).XYZtoLab(use_D65);