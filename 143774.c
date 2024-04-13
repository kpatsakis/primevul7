
      static double mp_polygon(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.listin.width());
        CImg<T> &img = ind==~0U?mp.imgout:mp.listout[ind];
        bool is_invalid_arguments = i_end<=4, is_outlined = false;
        if (!is_invalid_arguments) {
          int nbv = (int)_mp_arg(4);
          if (!nbv) is_invalid_arguments = true;
          else {
            if (nbv<0) { nbv = -nbv; is_outlined = true; }
            CImg<intT> points(nbv,2,1,1,0);
            CImg<T> color(img._spectrum,1,1,1,0);
            float opacity = 1;
            unsigned int i = 5, pattern=~0U;
            cimg_foroff(points,k) if (i<i_end) points(k/2,k%2) = (int)cimg::round(_mp_arg(i++));
            else { is_invalid_arguments = true; break; }
            if (!is_invalid_arguments) {
              if (i<i_end) opacity = (float)_mp_arg(i++);
              if (is_outlined && i<i_end) pattern = (unsigned int)_mp_arg(i++);
              cimg_forX(color,k) if (i<i_end) color[k] = (T)_mp_arg(i++);
              else { color.resize(k,1,1,1,-1); break; }
              color.resize(img._spectrum,1,1,1,0,2);
              if (is_outlined) img.draw_polygon(points,color._data,opacity,pattern);
              else img.draw_polygon(points,color._data,opacity);
            }
          }
        }
        if (is_invalid_arguments) {
          CImg<doubleT> args(i_end - 4);
          cimg_forX(args,k) args[k] = _mp_arg(4 + k);
          if (ind==~0U)
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'polygon()': "
                                        "Invalid arguments '%s'. ",
                                        mp.imgin.pixel_type(),args.value_string()._data);
          else
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'polygon()': "
                                        "Invalid arguments '#%u%s%s'. ",
                                        mp.imgin.pixel_type(),ind,args._width?",":"",args.value_string()._data);
        }
        return cimg::type<double>::nan();