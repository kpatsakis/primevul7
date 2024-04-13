    **/
    CImg<T>& load_cimg(const char *const filename, const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(filename);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));