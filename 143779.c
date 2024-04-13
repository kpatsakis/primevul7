    **/
    static CImgDisplay& empty() {
      static CImgDisplay _empty;
      return _empty.assign();