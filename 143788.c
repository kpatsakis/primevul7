    //! Convert pixel values from xyY to RGB color spaces \newinstance.
    CImg<Tuchar> get_xyYtoRGB(const bool use_D65=true) const {
      return CImg<Tuchar>(*this,false).xyYtoRGB(use_D65);