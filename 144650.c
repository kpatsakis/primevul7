
      static double mp_list_median(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        if (!mp.list_median) mp.list_median.assign(mp.listin._width);
        if (!mp.list_median[ind]) CImg<doubleT>::vector(mp.listin[ind].median()).move_to(mp.list_median[ind]);
        return *mp.list_median[ind];