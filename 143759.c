    //! Convert pixel values from xyY to RGB color spaces.
    CImg<T>& xyYtoRGB(const bool use_D65=true) {
      return xyYtoXYZ().XYZtoRGB(use_D65);