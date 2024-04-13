    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const T& val):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum,val);