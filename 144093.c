
      static double mp_image_sort(_cimg_math_parser& mp) {
        const bool is_increasing = (bool)_mp_arg(3);
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listout.width()),
          axis = (unsigned int)_mp_arg(4);
        cimg::mutex(6);
        CImg<T> &img = mp.listout[ind];
        img.sort(is_increasing,
                 axis==0 || axis=='x'?'x':
                 axis==1 || axis=='y'?'y':
                 axis==2 || axis=='z'?'z':
                 axis==3 || axis=='c'?'c':0);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();