    **/
    T& atNXYZC(const int pos, const int x, const int y, const int z, const int c) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZC(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZC(pos,x,y,z,c);