    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list) {
      list.assign(_width);
      bool is_one_shared_element = false;
      cimglist_for(*this,l) is_one_shared_element|=_data[l]._is_shared;
      if (is_one_shared_element) cimglist_for(*this,l) list[l].assign(_data[l]);
      else cimglist_for(*this,l) _data[l].move_to(list[l]);
      assign();
      return list;