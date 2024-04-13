    **/
    CImg<T>& mirror(const char axis) {
      if (is_empty()) return *this;
      T *pf, *pb, *buf = 0;
      switch (cimg::lowercase(axis)) {
      case 'x' : {
        pf = _data; pb = data(_width - 1);
        const unsigned int width2 = _width/2;
        for (unsigned int yzv = 0; yzv<_height*_depth*_spectrum; ++yzv) {
          for (unsigned int x = 0; x<width2; ++x) { const T val = *pf; *(pf++) = *pb; *(pb--) = val; }
          pf+=_width - width2;
          pb+=_width + width2;
        }
      } break;
      case 'y' : {
        buf = new T[_width];
        pf = _data; pb = data(0,_height - 1);
        const unsigned int height2 = _height/2;
        for (unsigned int zv = 0; zv<_depth*_spectrum; ++zv) {
          for (unsigned int y = 0; y<height2; ++y) {
            std::memcpy(buf,pf,_width*sizeof(T));
            std::memcpy(pf,pb,_width*sizeof(T));
            std::memcpy(pb,buf,_width*sizeof(T));
            pf+=_width;
            pb-=_width;
          }
          pf+=(ulongT)_width*(_height - height2);
          pb+=(ulongT)_width*(_height + height2);
        }
      } break;
      case 'z' : {
        buf = new T[(ulongT)_width*_height];
        pf = _data; pb = data(0,0,_depth - 1);
        const unsigned int depth2 = _depth/2;
        cimg_forC(*this,c) {
          for (unsigned int z = 0; z<depth2; ++z) {
            std::memcpy(buf,pf,_width*_height*sizeof(T));
            std::memcpy(pf,pb,_width*_height*sizeof(T));
            std::memcpy(pb,buf,_width*_height*sizeof(T));
            pf+=(ulongT)_width*_height;
            pb-=(ulongT)_width*_height;
          }
          pf+=(ulongT)_width*_height*(_depth - depth2);
          pb+=(ulongT)_width*_height*(_depth + depth2);
        }
      } break;
      case 'c' : {
        buf = new T[(ulongT)_width*_height*_depth];
        pf = _data; pb = data(0,0,0,_spectrum - 1);
        const unsigned int _spectrum2 = _spectrum/2;
        for (unsigned int v = 0; v<_spectrum2; ++v) {
          std::memcpy(buf,pf,_width*_height*_depth*sizeof(T));
          std::memcpy(pf,pb,_width*_height*_depth*sizeof(T));
          std::memcpy(pb,buf,_width*_height*_depth*sizeof(T));
          pf+=(ulongT)_width*_height*_depth;
          pb-=(ulongT)_width*_height*_depth;
        }
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "mirror(): Invalid specified axis '%c'.",
                                    cimg_instance,
                                    axis);
      }
      delete[] buf;
      return *this;