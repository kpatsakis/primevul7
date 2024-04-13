    //! Construct list as a copy of an existing list and force shared state of elements \inplace \specialization.
    CImgList<T>& assign(const CImgList<T>& list, const bool is_shared=false) {
      if (this==&list) return *this;
      CImgList<T> res(list._width);
      cimglist_for(res,l) res[l].assign(list[l],is_shared);
      return res.move_to(*this);