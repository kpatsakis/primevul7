    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum, const T& val) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum,val);
      return *this;