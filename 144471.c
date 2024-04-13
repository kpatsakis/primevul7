    template<typename t>
    CImgList<T>& assign(const CImg<t>& img, const bool is_shared=false) {
      assign(1);
      _data[0].assign(img,is_shared);
      return *this;