    **/
    CImg<T>& load_parrec(const char *const filename, const char axis='c', const float align=0) {
      CImgList<T> list;
      list.load_parrec(filename);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));