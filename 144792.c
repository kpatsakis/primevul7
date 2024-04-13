    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list, const unsigned int pos=~0U) {
      const unsigned int npos = pos>list._width?list._width:pos;
      move_to(list.insert(1,npos)[npos]);
      return list;