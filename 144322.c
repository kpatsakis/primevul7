    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height=1,
             const unsigned int depth=1, const unsigned int spectrum=1):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum);