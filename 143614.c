
      static double mp_list_is_shared(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        return (double)mp.listin[ind]._is_shared;