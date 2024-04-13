    template<typename t>
    CImgList<T>& operator,(const CImgList<t>& list) const {
      return (+*this).insert(list);