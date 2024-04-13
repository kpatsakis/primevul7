    template<typename t>
    CImgList<T>& operator,(const CImgList<t>& list) {
      return insert(list);