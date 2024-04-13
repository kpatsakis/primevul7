    //! Resize image to double-size, using the Scale2X algorithm \newinstance.
    CImg<T> get_resize_doubleXY() const {
#define _cimg_gs2x_for3(bound,i) \
 for (int i = 0, _p1##i = 0, \
      _n1##i = 1>=(bound)?(int)(bound) - 1:1; \
      _n1##i<(int)(bound) || i==--_n1##i; \
      _p1##i = i++, ++_n1##i, ptrd1+=(res)._width, ptrd2+=(res)._width)

#define _cimg_gs2x_for3x3(img,x,y,z,c,I,T) \
  _cimg_gs2x_for3((img)._height,y) for (int x = 0, \
   _p1##x = 0, \
   _n1##x = (int)( \
   (I[1] = (T)(img)(_p1##x,_p1##y,z,c)), \
   (I[3] = I[4] = (T)(img)(0,y,z,c)), \
   (I[7] = (T)(img)(0,_n1##y,z,c)),	\
   1>=(img)._width?(img).width() - 1:1); \
   (_n1##x<(img).width() && ( \
   (I[2] = (T)(img)(_n1##x,_p1##y,z,c)), \
   (I[5] = (T)(img)(_n1##x,y,z,c)), \
   (I[8] = (T)(img)(_n1##x,_n1##y,z,c)),1)) || \
   x==--_n1##x; \
   I[1] = I[2], \
   I[3] = I[4], I[4] = I[5], \
   I[7] = I[8], \
   _p1##x = x++, ++_n1##x)

      if (is_empty()) return *this;
      CImg<T> res(_width<<1,_height<<1,_depth,_spectrum);
      CImg_3x3(I,T);
      cimg_forZC(*this,z,c) {
        T
          *ptrd1 = res.data(0,0,z,c),
          *ptrd2 = ptrd1 + res._width;
        _cimg_gs2x_for3x3(*this,x,y,z,c,I,T) {
          if (Icp!=Icn && Ipc!=Inc) {
            *(ptrd1++) = Ipc==Icp?Ipc:Icc;
            *(ptrd1++) = Icp==Inc?Inc:Icc;
            *(ptrd2++) = Ipc==Icn?Ipc:Icc;
            *(ptrd2++) = Icn==Inc?Inc:Icc;
          } else { *(ptrd1++) = Icc; *(ptrd1++) = Icc; *(ptrd2++) = Icc; *(ptrd2++) = Icc; }
        }
      }
      return res;