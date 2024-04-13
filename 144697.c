    **/
    CImg<T> get_shared_rows(const unsigned int y0, const unsigned int y1,
                             const unsigned int z0=0, const unsigned int c0=0) {
      const unsigned int
        beg = (unsigned int)offset(0,y0,z0,c0),
        end = (unsigned int)offset(0,y1,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_rows(): Invalid request of a shared-memory subset "
                                    "(0->%u,%u->%u,%u,%u).",
                                    cimg_instance,
                                    _width - 1,y0,y1,z0,c0);

      return CImg<T>(_data + beg,_width,y1 - y0 + 1,1,1,true);