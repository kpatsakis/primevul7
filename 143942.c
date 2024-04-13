
      static double mp_isdir(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        const ulongT siz = (ulongT)mp.opcode[3];
        CImg<charT> ss(siz + 1);
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        return (double)cimg::is_directory(ss);