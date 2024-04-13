
      static double mp_set_Jxyz_v(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const double ox = mp.mem[_cimg_mp_slot_x], oy = mp.mem[_cimg_mp_slot_y], oz = mp.mem[_cimg_mp_slot_z];
        const int
          x = (int)(ox + _mp_arg(2)),
          y = (int)(oy + _mp_arg(3)),
          z = (int)(oz + _mp_arg(4));
        const double *ptrs = &_mp_arg(1) + 1;
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          const unsigned int vsiz = (unsigned int)mp.opcode[5];
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_for_inC(img,0,vsiz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();