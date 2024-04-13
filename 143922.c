    //! Convert pixel values from RGB to xyY color spaces.
    CImg<T>& RGBtoxyY(const bool use_D65=true) {
      return RGBtoXYZ(use_D65).XYZtoxyY();