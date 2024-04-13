    template<typename t>
    CImgList<_cimg_Tt> operator,(const CImgList<t>& list) const {
      return CImgList<_cimg_Tt>(list,false).insert(*this,0);