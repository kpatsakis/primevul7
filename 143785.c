     **/
    T& at(const int offset) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "at(): Empty instance.",
                                    cimg_instance);
      return _at(offset);