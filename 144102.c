    //! Convert pixel values from Lab to RGB color spaces.
    CImg<T>& LabtoRGB(const bool use_D65=true) {
      return LabtoXYZ().XYZtoRGB(use_D65);