    **/
    const CImg<T>& operator()(const unsigned int pos) const {
      return const_cast<CImgList<T>*>(this)->operator()(pos);