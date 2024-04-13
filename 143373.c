
      static double mp_list_set_ioff(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        CImg<T> &img = mp.listout[ind];
        const longT
          off = (longT)_mp_arg(3),
          whds = (longT)img.size();
        const double val = _mp_arg(1);
        if (off>=0 && off<whds) img[off] = (T)val;
        return val;