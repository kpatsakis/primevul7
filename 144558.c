    **/
    CImg<T> get_shared_channels(const unsigned int c0, const unsigned int c1) {
      const unsigned int
        beg = (unsigned int)offset(0,0,0,c0),
        end = (unsigned int)offset(0,0,0,c1);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_channels(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,0->%u,%u->%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,_depth - 1,c0,c1);

      return CImg<T>(_data + beg,_width,_height,_depth,c1 - c0 + 1,true);