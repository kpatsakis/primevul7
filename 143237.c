    //! Access to a pixel value at a specified offset, using Neumann boundary conditions \const.
    const T& at(const int offset) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "at(): Empty instance.",
                                    cimg_instance);
      return _at(offset);