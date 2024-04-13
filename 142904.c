
      static double mp_image_resize(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listout.width());
        cimg::mutex(6);
        CImg<T> &img = mp.listout[ind];
        const double
          _w = mp.opcode[3]==~0U?-100:_mp_arg(3),
          _h = mp.opcode[4]==~0U?-100:_mp_arg(4),
          _d = mp.opcode[5]==~0U?-100:_mp_arg(5),
          _s = mp.opcode[6]==~0U?-100:_mp_arg(6);
        const unsigned int
          w = (unsigned int)(_w>=0?_w:-_w*img.width()/100),
          h = (unsigned int)(_h>=0?_h:-_h*img.height()/100),
          d = (unsigned int)(_d>=0?_d:-_d*img.depth()/100),
          s = (unsigned int)(_s>=0?_s:-_s*img.spectrum()/100),
          interp = (int)_mp_arg(7);
        if (mp.is_fill && img._data==mp.imgout._data) {
          cimg::mutex(6,0);
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'resize()': "
                                      "Cannot both fill and resize image (%u,%u,%u,%u) "
                                      "to new dimensions (%u,%u,%u,%u).",
                                      img.pixel_type(),img._width,img._height,img._depth,img._spectrum,w,h,d,s);
        }
        const unsigned int
          boundary = (int)_mp_arg(8);
        const float
          cx = (float)_mp_arg(9),
          cy = (float)_mp_arg(10),
          cz = (float)_mp_arg(11),
          cc = (float)_mp_arg(12);
        img.resize(w,h,d,s,interp,boundary,cx,cy,cz,cc);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();