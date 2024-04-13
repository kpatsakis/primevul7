    template<typename t>
    CImg<_cimg_Tt> get_max(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).max(img);