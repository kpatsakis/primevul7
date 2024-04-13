    //! Convert pixel values from RGB to YUV color spaces \newinstance.
    CImg<Tfloat> get_RGBtoYUV() const {
      return CImg<Tfloat>(*this,false).RGBtoYUV();