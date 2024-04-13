    //! Append two images along specified axis \specialization.
    CImg<T>& append(const CImg<T>& img, const char axis='x', const float align=0) {
      if (is_empty()) return assign(img,false);
      if (!img) return *this;
      return CImgList<T>(*this,img,true).get_append(axis,align).move_to(*this);