    **/
    CImgDisplay& set_normalization(const unsigned int normalization) {
      _normalization = normalization;
      _min = _max = 0;
      return *this;