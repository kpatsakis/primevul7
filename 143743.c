    **/
    ~CImg() {
      if (!_is_shared) delete[] _data;