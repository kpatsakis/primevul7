    //! Convert pixel values from RGB to YCbCr color spaces \newinstance.
    CImg<Tuchar> get_YCbCrtoRGB() const {
      return CImg<Tuchar>(*this,false).YCbCrtoRGB();