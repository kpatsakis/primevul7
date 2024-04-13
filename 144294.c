    //! Pointwise max operator between an image and an expression \newinstance.
    CImg<Tfloat> get_max(const char *const expression) const {
      return CImg<Tfloat>(*this,false).max(expression);