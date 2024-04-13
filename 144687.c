    //! Convert pixel values from RGB to CMYK color spaces \newinstance.
    CImg<Tfloat> get_RGBtoCMYK() const {
      return CImg<Tfloat>(*this,false).RGBtoCMYK();