    **/
    CImgList<T>& swap(CImgList<T>& list) {
      cimg::swap(_width,list._width,_allocated_width,list._allocated_width);
      cimg::swap(_data,list._data);
      return list;