    //! Access to pixel value with Neumann boundary conditions for the 4 coordinates (\c pos, \c x,\c y,\c z) \const.
    T atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZ(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZ(pos,x,y,z,c);