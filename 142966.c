
      static double mp_set_Ixyz_s(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const int
          x = (int)_mp_arg(2),
          y = (int)_mp_arg(3),
          z = (int)_mp_arg(4);
        const T val = (T)_mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_forC(img,c) { *ptrd = val; ptrd+=whd; }
        }
        return _mp_arg(1);