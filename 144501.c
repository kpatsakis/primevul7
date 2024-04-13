    //! Raise each pixel value to a specified power \newinstance.
    CImg<Tfloat> get_pow(const double p) const {
      return CImg<Tfloat>(*this,false).pow(p);