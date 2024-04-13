    template<typename t>
    CImg<_cimg_Tt> operator%(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false)%=img;