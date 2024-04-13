    //! Convert pixel values from RGB to YCbCr color spaces \newinstance.
    CImg<Tuchar> get_RGBtoYCbCr() const {
      return CImg<Tuchar>(*this,false).RGBtoYCbCr();