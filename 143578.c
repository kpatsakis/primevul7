
      static double mp_list_whds(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        return (double)mp.listin[ind]._width*mp.listin[ind]._height*mp.listin[ind]._depth*mp.listin[ind]._spectrum;