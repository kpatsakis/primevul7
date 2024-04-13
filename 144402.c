    //! Convert pixel values from RGB to CMY color spaces \newinstance.
    CImg<Tuchar> get_RGBtoCMY() const {
      return CImg<Tfloat>(*this,false).RGBtoCMY();