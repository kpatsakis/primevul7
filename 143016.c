
      static double mp_list_set_jxyzc(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        CImg<T> &img = mp.listout[ind];
        const double
          ox = mp.mem[_cimg_mp_slot_x], oy = mp.mem[_cimg_mp_slot_y],
          oz = mp.mem[_cimg_mp_slot_z], oc = mp.mem[_cimg_mp_slot_c];
        const int
          x = (int)(ox + _mp_arg(3)), y = (int)(oy + _mp_arg(4)),
          z = (int)(oz + _mp_arg(5)), c = (int)(oc + _mp_arg(6));
        const double val = _mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() &&
            z>=0 && z<img.depth() && c>=0 && c<img.spectrum())
          img(x,y,z,c) = (T)val;
        return val;