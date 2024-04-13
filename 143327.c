    **/
    CImgList<T>& assign() {
      delete[] _data;
      _width = _allocated_width = 0;
      _data = 0;
      return *this;