
      static double mp_display(_cimg_math_parser& mp) {
        const unsigned int
          _siz = (unsigned int)mp.opcode[3],
          siz = _siz?_siz:1;
        const double *const ptr = &_mp_arg(1) + (_siz?1:0);
        const int
          w = (int)_mp_arg(4),
          h = (int)_mp_arg(5),
          d = (int)_mp_arg(6),
          s = (int)_mp_arg(7);
        CImg<doubleT> img;
        if (w>0 && h>0 && d>0 && s>0) {
          if ((unsigned int)w*h*d*s<=siz) img.assign(ptr,w,h,d,s,true);
          else img.assign(ptr,siz).resize(w,h,d,s,-1);
        } else img.assign(ptr,1,siz,1,1,true);

        CImg<charT> expr(mp.opcode[2] - 8);
        const ulongT *ptrs = mp.opcode._data + 8;
        cimg_for(expr,ptrd,char) *ptrd = (char)*(ptrs++);
        ((CImg<charT>::string("[" cimg_appname "_math_parser] ",false,true),expr)>'x').move_to(expr);
        cimg::strellipsize(expr);
        std::fputc('\n',cimg::output());
        img.display(expr._data);
        return cimg::type<double>::nan();