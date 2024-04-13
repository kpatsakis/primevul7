    **/
    T& atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNX(): Empty instance.",
                                    cimglist_instance);

      return _atNX(pos,x,y,z,c);