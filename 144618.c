    template<typename t>
    CImg<_cimg_Tt> get_min(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).min(img);