    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list, const unsigned int pos) {
      if (is_empty()) return list;
      const unsigned int npos = pos>list._width?list._width:pos;
      list.insert(_width,npos);
      bool is_one_shared_element = false;
      cimglist_for(*this,l) is_one_shared_element|=_data[l]._is_shared;
      if (is_one_shared_element) cimglist_for(*this,l) list[npos + l].assign(_data[l]);
      else cimglist_for(*this,l) _data[l].move_to(list[npos + l]);
      assign();
      return list;