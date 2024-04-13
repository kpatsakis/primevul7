    **/
    CImg<ucharT> get_serialize(const bool is_compressed=false) const {
      return CImgList<T>(*this,true).get_serialize(is_compressed);