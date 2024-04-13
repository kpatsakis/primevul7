    //! Pointwise min operator between an image and an expression \newinstance.
    CImg<Tfloat> get_min(const char *const expression) const {
      return CImg<Tfloat>(*this,false).min(expression);