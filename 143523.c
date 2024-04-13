    template<typename t>
    CImg<T>& assign(const CImg<t>& img) {
      return assign(img._data,img._width,img._height,img._depth,img._spectrum);