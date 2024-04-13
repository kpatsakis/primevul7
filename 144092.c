    //! Raise each pixel value to a power, specified from an expression \newinstance.
    CImg<Tfloat> get_pow(const char *const expression) const {
      return CImg<Tfloat>(*this,false).pow(expression);