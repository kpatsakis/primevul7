    template<typename t>
    CImg<_cimg_Tt> get_cross(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this).cross(img);