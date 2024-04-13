    template<typename t>
    CImg<_cimg_Tt> get_mul(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).mul(img);