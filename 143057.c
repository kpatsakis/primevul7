    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      if (is_empty()) { img.assign(); return *this; }
      const unsigned int *ptrs = _data;
      img.assign(_width,_height,1,3);
      T
        *data1 = img.data(0,0,0,0),
        *data2 = img.data(0,0,0,1),
        *data3 = img.data(0,0,0,2);
      for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
        const unsigned int val = *(ptrs++);
        *(data1++) = (T)(unsigned char)(val>>16);
        *(data2++) = (T)(unsigned char)((val>>8)&0xFF);
        *(data3++) = (T)(unsigned char)(val&0xFF);
      }
      return *this;