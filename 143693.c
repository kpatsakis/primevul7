    template<typename t>
    CImgList<T>& operator,(const CImg<t>& img) {
      return insert(img);