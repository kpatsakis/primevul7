    template<typename t>
    CImgList(const unsigned int n, const CImg<t>& img, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(img,is_shared);