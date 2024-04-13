    template<typename t>
    CImgList<T>& assign(const CImgList<t>& list, const bool is_shared=false) {
      cimg::unused(is_shared);
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],false);
      return *this;