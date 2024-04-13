    template<typename t>
    CImg<T> get_discard(const CImg<t>& values, const char axis=0) const {
      CImg<T> res;
      if (!values) return +*this;
      if (is_empty()) return res;
      const ulongT vsiz = values.size();
      const char _axis = cimg::lowercase(axis);
      ulongT j = 0;
      unsigned int k = 0;
      int i0 = 0;
      res.assign(width(),height(),depth(),spectrum());
      switch (_axis) {
      case 'x' : {
        cimg_forX(*this,i) {
          if ((*this)(i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(k,get_columns(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<width()) { res.draw_image(k,get_columns(i0,width() - 1)); k+=width() - i0; }
        res.resize(k,-100,-100,-100,0);
      } break;
      case 'y' : {
        cimg_forY(*this,i) {
          if ((*this)(0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,k,get_rows(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<height()) { res.draw_image(0,k,get_rows(i0,height() - 1)); k+=height() - i0; }
        res.resize(-100,k,-100,-100,0);
      } break;
      case 'z' : {
        cimg_forZ(*this,i) {
          if ((*this)(0,0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,0,k,get_slices(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<depth()) { res.draw_image(0,0,k,get_slices(i0,height() - 1)); k+=depth() - i0; }
        res.resize(-100,-100,k,-100,0);
      } break;
      case 'c' : {
        cimg_forC(*this,i) {
          if ((*this)(0,0,0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,0,0,k,get_channels(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<spectrum()) { res.draw_image(0,0,k,get_channels(i0,height() - 1)); k+=spectrum() - i0; }
        res.resize(-100,-100,-100,k,0);
      } break;
      default : {
        res.unroll('y');
        cimg_foroff(*this,i) {
          if ((*this)[i]!=(T)values[j]) {
            if (j) --i;
            std::memcpy(res._data + k,_data + i0,(i - i0 + 1)*sizeof(T));
            k+=i - i0 + 1; i0 = (int)i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = (int)i + 1; }}
        }
        const ulongT siz = size();
        if ((ulongT)i0<siz) { std::memcpy(res._data + k,_data + i0,(siz - i0)*sizeof(T)); k+=siz - i0; }
        res.resize(1,k,1,1,0);
      }
      }
      return res;