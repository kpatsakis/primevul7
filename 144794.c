    template<typename t>
    CImgList<T>& assign(const unsigned int n, const CImg<t>& img, const bool is_shared=false) {
      assign(n);
      cimglist_apply(*this,assign)(img,is_shared);
      return *this;