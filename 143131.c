
      static double mp_set_Joff_v(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const int
          ox = (int)mp.mem[_cimg_mp_slot_x], oy = (int)mp.mem[_cimg_mp_slot_y],
          oz = (int)mp.mem[_cimg_mp_slot_z], oc = (int)mp.mem[_cimg_mp_slot_c];
        const longT
          off = img.offset(ox,oy,oz,oc) + (longT)_mp_arg(2),
          whd = (longT)img.width()*img.height()*img.depth();
        const double *ptrs = &_mp_arg(1) + 1;
        if (off>=0 && off<whd) {
          const unsigned int vsiz = (unsigned int)mp.opcode[3];
          T *ptrd = &img[off];
          cimg_for_inC(img,0,vsiz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();