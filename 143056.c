    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      if (is_empty()) { img.assign(); return *this; }
      const unsigned char *ptrs = (unsigned char*)_data;
      img.assign(_width,_height,1,3);
      T
        *data1 = img.data(0,0,0,0),
        *data2 = img.data(0,0,0,1),
        *data3 = img.data(0,0,0,2);
      if (cimg::X11_attr().is_blue_first) cimg::swap(data1,data3);
      switch (cimg::X11_attr().nb_bits) {
      case 8 : {
        for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned char val = *(ptrs++);
          *(data1++) = (T)(val&0xe0);
          *(data2++) = (T)((val&0x1c)<<3);
          *(data3++) = (T)(val<<6);
        }
      } break;
      case 16 : {
        if (cimg::X11_attr().byte_order) for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned char
            val0 = ptrs[0],
            val1 = ptrs[1];
          ptrs+=2;
          *(data1++) = (T)(val0&0xf8);
          *(data2++) = (T)((val0<<5) | ((val1&0xe0)>>5));
          *(data3++) = (T)(val1<<3);
          } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned short
            val0 = ptrs[0],
            val1 = ptrs[1];
          ptrs+=2;
          *(data1++) = (T)(val1&0xf8);
          *(data2++) = (T)((val1<<5) | ((val0&0xe0)>>5));
          *(data3++) = (T)(val0<<3);
        }
      } break;
      default : {
        if (cimg::X11_attr().byte_order) for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          ++ptrs;
          *(data1++) = (T)ptrs[0];
          *(data2++) = (T)ptrs[1];
          *(data3++) = (T)ptrs[2];
          ptrs+=3;
          } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            *(data3++) = (T)ptrs[0];
            *(data2++) = (T)ptrs[1];
            *(data1++) = (T)ptrs[2];
            ptrs+=3;
            ++ptrs;
          }
      }
      }
      return *this;