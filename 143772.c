    template<typename t>
    CImgList<_cimg_Tt> operator,(const CImg<t>& img) const {
      return CImgList<_cimg_Tt>(*this,img);