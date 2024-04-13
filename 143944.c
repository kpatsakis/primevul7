    template<typename t>
    CImg<T>& assign(const CImg<t>& img, const char *const dimensions) {
      if (!dimensions || !*dimensions) return assign(img._width,img._height,img._depth,img._spectrum);
      unsigned int siz[4] = { 0,1,1,1 }, k = 0;
      CImg<charT> item(256);
      for (const char *s = dimensions; *s && k<4; ++k) {
        if (cimg_sscanf(s,"%255[^0-9%xyzvwhdcXYZVWHDC]",item._data)>0) s+=std::strlen(item);
        if (*s) {
          unsigned int val = 0; char sep = 0;
          if (cimg_sscanf(s,"%u%c",&val,&sep)>0) {
            if (sep=='%') siz[k] = val*(k==0?_width:k==1?_height:k==2?_depth:_spectrum)/100;
            else siz[k] = val;
            while (*s>='0' && *s<='9') ++s;
            if (sep=='%') ++s;
          } else switch (cimg::lowercase(*s)) {
          case 'x' : case 'w' : siz[k] = img._width; ++s; break;
          case 'y' : case 'h' : siz[k] = img._height; ++s; break;
          case 'z' : case 'd' : siz[k] = img._depth; ++s; break;
          case 'c' : case 's' : siz[k] = img._spectrum; ++s; break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "assign(): Invalid character '%c' detected in specified dimension string '%s'.",
                                        cimg_instance,
                                        *s,dimensions);
          }
        }
      }
      return assign(siz[0],siz[1],siz[2],siz[3]);