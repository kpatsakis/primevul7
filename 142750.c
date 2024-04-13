
      static double mp_set_Jxyz_s(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const double ox = mp.mem[_cimg_mp_slot_x], oy = mp.mem[_cimg_mp_slot_y], oz = mp.mem[_cimg_mp_slot_z];
        const int
          x = (int)(ox + _mp_arg(2)),
          y = (int)(oy + _mp_arg(3)),
          z = (int)(oz + _mp_arg(4));
        const T val = (T)_mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_forC(img,c) { *ptrd = val; ptrd+=whd; }
        }
        return _mp_arg(1);