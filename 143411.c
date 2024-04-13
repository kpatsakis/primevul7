    //! Convert pixel values from RGB to Lab color spaces.
    CImg<T>& RGBtoLab(const bool use_D65=true) {
      return RGBtoXYZ(use_D65).XYZtoLab(use_D65);