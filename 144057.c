    template<typename t>
    CImg<_cimg_Tt> get_append(const CImg<T>& img, const char axis='x', const float align=0) const {
      if (is_empty()) return +img;
      if (!img) return +*this;
      return CImgList<_cimg_Tt>(*this,true).insert(img).get_append(axis,align);