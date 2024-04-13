    template<typename t>
    CImg<Tfloat> get_pow(const CImg<t>& img) const {
      return CImg<Tfloat>(*this,false).pow(img);