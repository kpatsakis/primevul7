    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height=1,
                        const unsigned int depth=1, const unsigned int spectrum=1) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum);
      return *this;