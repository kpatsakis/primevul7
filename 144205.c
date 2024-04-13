    //! Access to pixel value with Neumann boundary conditions for the 3 coordinates (\c pos, \c x,\c y) \const.
    T atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXY(): Empty instance.",
                                    cimglist_instance);

      return _atNXY(pos,x,y,z,c);