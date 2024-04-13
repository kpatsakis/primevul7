    **/
    CImg<T>& swap(CImg<T>& img) {
      cimg::swap(_width,img._width,_height,img._height,_depth,img._depth,_spectrum,img._spectrum);
      cimg::swap(_data,img._data);
      cimg::swap(_is_shared,img._is_shared);
      return img;