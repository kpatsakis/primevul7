
      static double mp_set_ixyzc(_cimg_math_parser& mp) {
        CImg<T> &img = mp.imgout;
        const int
          x = (int)_mp_arg(2), y = (int)_mp_arg(3),
          z = (int)_mp_arg(4), c = (int)_mp_arg(5);
        const double val = _mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() &&
            z>=0 && z<img.depth() && c>=0 && c<img.spectrum())
          img(x,y,z,c) = (T)val;
        return val;