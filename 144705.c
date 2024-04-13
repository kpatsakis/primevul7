
      static double mp_set_joff(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const int
          ox = (int)mp.mem[_cimg_mp_slot_x], oy = (int)mp.mem[_cimg_mp_slot_y],
          oz = (int)mp.mem[_cimg_mp_slot_z], oc = (int)mp.mem[_cimg_mp_slot_c];
        const longT
          off = img.offset(ox,oy,oz,oc) + (longT)_mp_arg(2),
          whds = (longT)img.size();
        const double val = _mp_arg(1);
        if (off>=0 && off<whds) img[off] = (T)val;
        return val;