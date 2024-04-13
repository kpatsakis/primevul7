    template<typename t>
    CImg<_cimg_Tt> get_div(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).div(img);