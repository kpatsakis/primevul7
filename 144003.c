    template<typename t>
    CImg<_cimg_Tt> operator/(const t value) const {
      return CImg<_cimg_Tt>(*this,false)/=value;