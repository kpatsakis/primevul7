    //! Construct list from another list \specialization.
    CImgList<T>& operator=(const CImgList<T>& list) {
      return assign(list);