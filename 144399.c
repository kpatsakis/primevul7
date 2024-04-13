    template<typename t1, typename t2>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const bool is_shared=false) {
      assign(2);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared);
      return *this;