    //! Convert pixel values from XYZ to xyY color spaces \newinstance.
    CImg<Tfloat> get_XYZtoxyY() const {
      return CImg<Tfloat>(*this,false).XYZtoxyY();