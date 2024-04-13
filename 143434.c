
      static double mp_list_set_Ixyz_v(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        CImg<T> &img = mp.listout[ind];
        const int
          x = (int)_mp_arg(3),
          y = (int)_mp_arg(4),
          z = (int)_mp_arg(5);
        const double *ptrs = &_mp_arg(1) + 1;
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          const unsigned int vsiz = (unsigned int)mp.opcode[6];
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_for_inC(img,0,vsiz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();