    template<typename t>
    CImgList<T> operator,(const CImg<t>& img) const {
      return (+*this).insert(img);