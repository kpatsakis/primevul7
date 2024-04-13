    template<typename t>
    CImg<_cimg_Tt> operator/(const CImg<t>& img) const {
      return (*this)*img.get_invert();