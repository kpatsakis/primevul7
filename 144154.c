
      static double mp_list_set_ixyzc(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        CImg<T> &img = mp.listout[ind];
        const int
          x = (int)_mp_arg(3), y = (int)_mp_arg(4),
          z = (int)_mp_arg(5), c = (int)_mp_arg(6);
        const double val = _mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() &&
            z>=0 && z<img.depth() && c>=0 && c<img.spectrum())
          img(x,y,z,c) = (T)val;
        return val;