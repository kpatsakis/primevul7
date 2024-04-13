
      static double mp_set_Ioff_s(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const longT
          off = (longT)_mp_arg(2),
          whd = (longT)img.width()*img.height()*img.depth();
        const T val = (T)_mp_arg(1);
        if (off>=0 && off<whd) {
          T *ptrd = &img[off];
          cimg_forC(img,c) { *ptrd = val; ptrd+=whd; }
        }
        return _mp_arg(1);