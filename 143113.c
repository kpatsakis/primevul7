    //! Construct list copy \specialization.
    CImgList(const CImgList<T>& list):_width(0),_allocated_width(0),_data(0) {
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],list[l]._is_shared);