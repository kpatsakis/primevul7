    template<typename t>
    CImg<T>& append(const CImg<t>& img, const char axis='x', const float align=0) {
      if (is_empty()) return assign(img,false);
      if (!img) return *this;
      return CImgList<T>(*this,true).insert(img).get_append(axis,align).move_to(*this);