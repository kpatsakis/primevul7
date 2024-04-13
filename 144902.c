    **/
    CImgList<T> operator+() const {
      return CImgList<T>(*this,false);