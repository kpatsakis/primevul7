    //! Append two images along specified axis \specialization.
    CImg<T> get_append(const CImg<T>& img, const char axis='x', const float align=0) const {
      if (is_empty()) return +img;
      if (!img) return +*this;
      return CImgList<T>(*this,img,true).get_append(axis,align);