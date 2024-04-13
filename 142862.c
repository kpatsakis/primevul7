    **/
    CImg<T>& at(const int pos) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "at(): Empty instance.",
                                    cimglist_instance);

      return _data[cimg::cut(pos,0,width() - 1)];