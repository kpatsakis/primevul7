    **/
    CImg<Tfloat> operator%(const char *const expression) const {
      return CImg<Tfloat>(*this,false)%=expression;