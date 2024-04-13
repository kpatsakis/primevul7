    **/
    const CImg<T>& back() const {
      return *(_data + _width - 1);