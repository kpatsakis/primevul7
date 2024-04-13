    template<typename t>
    CImg<t> _permute_axes(const char *const order, const t&) const {
      if (is_empty() || !order) return CImg<t>(*this,false);
      CImg<t> res;
      const T* ptrs = _data;
      unsigned char s_code[4] = { 0,1,2,3 }, n_code[4] = { 0 };
      for (unsigned int l = 0; order[l]; ++l) {
        int c = cimg::lowercase(order[l]);
        if (c!='x' && c!='y' && c!='z' && c!='c') { *s_code = 4; break; }
        else { ++n_code[c%=4]; s_code[l] = c; }
      }
      if (*order && *s_code<4 && *n_code<=1 && n_code[1]<=1 && n_code[2]<=1 && n_code[3]<=1) {
        const unsigned int code = (s_code[0]<<12) | (s_code[1]<<8) | (s_code[2]<<4) | (s_code[3]);
        ulongT wh, whd;
        switch (code) {
        case 0x0123 : // xyzc
          return +*this;
        case 0x0132 : // xycz
          res.assign(_width,_height,_spectrum,_depth);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(x,y,c,z,wh,whd) = (t)*(ptrs++);
          break;
        case 0x0213 : // xzyc
          res.assign(_width,_depth,_height,_spectrum);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(x,z,y,c,wh,whd) = (t)*(ptrs++);
          break;
        case 0x0231 : // xzcy
          res.assign(_width,_depth,_spectrum,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(x,z,c,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x0312 : // xcyz
          res.assign(_width,_spectrum,_height,_depth);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(x,c,y,z,wh,whd) = (t)*(ptrs++);
          break;
        case 0x0321 : // xczy
          res.assign(_width,_spectrum,_depth,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(x,c,z,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x1023 : // yxzc
          res.assign(_height,_width,_depth,_spectrum);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,x,z,c,wh,whd) = (t)*(ptrs++);
          break;
        case 0x1032 : // yxcz
          res.assign(_height,_width,_spectrum,_depth);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,x,c,z,wh,whd) = (t)*(ptrs++);
          break;
        case 0x1203 : // yzxc
          res.assign(_height,_depth,_width,_spectrum);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,z,x,c,wh,whd) = (t)*(ptrs++);
          break;
        case 0x1230 : // yzcx
          res.assign(_height,_depth,_spectrum,_width);
          switch (_width) {
          case 1 : {
            t *ptr_r = res.data(0,0,0,0);
            for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
              *(ptr_r++) = (t)*(ptrs++);
            }
          } break;
          case 2 : {
            t *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1);
            for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
              *(ptr_r++) = (t)ptrs[0];
              *(ptr_g++) = (t)ptrs[1];
              ptrs+=2;
            }
          } break;
          case 3 : { // Optimization for the classical conversion from interleaved RGB to planar RGB
            t *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1), *ptr_b = res.data(0,0,0,2);
            for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
              *(ptr_r++) = (t)ptrs[0];
              *(ptr_g++) = (t)ptrs[1];
              *(ptr_b++) = (t)ptrs[2];
              ptrs+=3;
            }
          } break;
          case 4 : { // Optimization for the classical conversion from interleaved RGBA to planar RGBA
            t
              *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1),
              *ptr_b = res.data(0,0,0,2), *ptr_a = res.data(0,0,0,3);
            for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
              *(ptr_r++) = (t)ptrs[0];
              *(ptr_g++) = (t)ptrs[1];
              *(ptr_b++) = (t)ptrs[2];
              *(ptr_a++) = (t)ptrs[3];
              ptrs+=4;
            }
          } break;
          default : {
            wh = (ulongT)res._width*res._height; whd = wh*res._depth;
            cimg_forXYZC(*this,x,y,z,c) res(y,z,c,x,wh,whd) = *(ptrs++);
            return res;
          }
          }
          break;
        case 0x1302 : // ycxz
          res.assign(_height,_spectrum,_width,_depth);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,c,x,z,wh,whd) = (t)*(ptrs++);
          break;
        case 0x1320 : // yczx
          res.assign(_height,_spectrum,_depth,_width);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,c,z,x,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2013 : // zxyc
          res.assign(_depth,_width,_height,_spectrum);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,x,y,c,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2031 : // zxcy
          res.assign(_depth,_width,_spectrum,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,x,c,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2103 : // zyxc
          res.assign(_depth,_height,_width,_spectrum);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,y,x,c,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2130 : // zycx
          res.assign(_depth,_height,_spectrum,_width);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,y,c,x,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2301 : // zcxy
          res.assign(_depth,_spectrum,_width,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,c,x,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x2310 : // zcyx
          res.assign(_depth,_spectrum,_height,_width);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(z,c,y,x,wh,whd) = (t)*(ptrs++);
          break;
        case 0x3012 : // cxyz
          res.assign(_spectrum,_width,_height,_depth);
          switch (_spectrum) {
          case 1 : {
            const T *ptr_r = data(0,0,0,0);
            t *ptrd = res._data;
            for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) *(ptrd++) = (t)*(ptr_r++);
          } break;
          case 2 : {
            const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1);
            t *ptrd = res._data;
            for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
              ptrd[0] = (t)*(ptr_r++);
              ptrd[1] = (t)*(ptr_g++);
              ptrd+=2;
            }
          } break;
          case 3 : { // Optimization for the classical conversion from planar RGB to interleaved RGB
            const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
            t *ptrd = res._data;
            for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
              ptrd[0] = (t)*(ptr_r++);
              ptrd[1] = (t)*(ptr_g++);
              ptrd[2] = (t)*(ptr_b++);
              ptrd+=3;
            }
          } break;
          case 4 : { // Optimization for the classical conversion from planar RGBA to interleaved RGBA
            const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
            t *ptrd = res._data;
            for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
              ptrd[0] = (t)*(ptr_r++);
              ptrd[1] = (t)*(ptr_g++);
              ptrd[2] = (t)*(ptr_b++);
              ptrd[3] = (t)*(ptr_a++);
              ptrd+=4;
            }
          } break;
          default : {
            wh = (ulongT)res._width*res._height; whd = wh*res._depth;
            cimg_forXYZC(*this,x,y,z,c) res(c,x,y,z,wh,whd) = (t)*(ptrs++);
          }
          }
          break;
        case 0x3021 : // cxzy
          res.assign(_spectrum,_width,_depth,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,x,z,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x3102 : // cyxz
          res.assign(_spectrum,_height,_width,_depth);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,y,x,z,wh,whd) = (t)*(ptrs++);
          break;
        case 0x3120 : // cyzx
          res.assign(_spectrum,_height,_depth,_width);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,y,z,x,wh,whd) = (t)*(ptrs++);
          break;
        case 0x3201 : // czxy
          res.assign(_spectrum,_depth,_width,_height);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,z,x,y,wh,whd) = (t)*(ptrs++);
          break;
        case 0x3210 : // czyx
          res.assign(_spectrum,_depth,_height,_width);
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,z,y,x,wh,whd) = (t)*(ptrs++);
          break;
        }
      }
      if (!res)
        throw CImgArgumentException(_cimg_instance
                                    "permute_axes(): Invalid specified permutation '%s'.",
                                    cimg_instance,
                                    order);
      return res;