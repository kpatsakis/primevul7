    //! Load image from a .cimg[z] file \overloading.
    CImg<T>& load_cimg(std::FILE *const file, const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(file);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));