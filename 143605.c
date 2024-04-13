    template<typename t>
    CImgList(const CImgList<t>& list, const bool is_shared):_width(0),_allocated_width(0),_data(0) {
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],is_shared);