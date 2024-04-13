
      static double mp_list_set_Ioff_v(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        CImg<T> &img = mp.listout[ind];
        const longT
          off = (longT)_mp_arg(3),
          whd = (longT)img.width()*img.height()*img.depth();
        const double *ptrs = &_mp_arg(1) + 1;
        if (off>=0 && off<whd) {
          const unsigned int vsiz = (unsigned int)mp.opcode[4];
          T *ptrd = &img[off];
          cimg_for_inC(img,0,vsiz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();