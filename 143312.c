    template<typename t>
    CImg<Tfloat> get_atan2(const CImg<t>& img) const {
      return CImg<Tfloat>(*this,false).atan2(img);