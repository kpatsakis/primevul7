
      static double mp_set_ioff(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const longT
          off = (longT)_mp_arg(2),
          whds = (longT)img.size();
        const double val = _mp_arg(1);
        if (off>=0 && off<whds) img[off] = (T)val;
        return val;