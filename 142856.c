    **/
    static CImgList<T>& empty() {
      static CImgList<T> _empty;
      return _empty.assign();