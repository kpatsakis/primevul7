    //! Return a shared memory image referencing a range of slices of the image instance \const.
    const CImg<T> get_shared_slices(const unsigned int z0, const unsigned int z1, const unsigned int c0=0) const {
      const unsigned int
        beg = (unsigned int)offset(0,0,z0,c0),
        end = (unsigned int)offset(0,0,z1,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_slices(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,%u->%u,%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,z0,z1,c0);

      return CImg<T>(_data + beg,_width,_height,z1 - z0 + 1,1,true);