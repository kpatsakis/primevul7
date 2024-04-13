     **/
    CImg<T> get_shared_points(const unsigned int x0, const unsigned int x1,
                              const unsigned int y0=0, const unsigned int z0=0, const unsigned int c0=0) {
      const unsigned int
        beg = (unsigned int)offset(x0,y0,z0,c0),
        end = (unsigned int)offset(x1,y0,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_points(): Invalid request of a shared-memory subset (%u->%u,%u,%u,%u).",
                                    cimg_instance,
                                    x0,x1,y0,z0,c0);

      return CImg<T>(_data + beg,x1 - x0 + 1,1,1,1,true);