    **/
    ~CImgDisplay() {
      assign();
      delete[] _keys;
      delete[] _released_keys;