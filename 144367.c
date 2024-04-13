    **/
    T& atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atN(): Empty instance.",
                                    cimglist_instance);
      return _atN(pos,x,y,z,c);