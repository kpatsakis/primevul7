    //! Discard neighboring duplicates in the image buffer, along the specified axis \newinstance.
    CImg<T> get_discard(const char axis=0) const {
      CImg<T> res;
      if (is_empty()) return res;
      const char _axis = cimg::lowercase(axis);
      T current = *_data?(T)0:(T)1;
      int j = 0;
      res.assign(width(),height(),depth(),spectrum());
      switch (_axis) {
      case 'x' : {
        cimg_forX(*this,i)
          if ((*this)(i)!=current) { res.draw_image(j++,get_column(i)); current = (*this)(i); }
        res.resize(j,-100,-100,-100,0);
      } break;
      case 'y' : {
        cimg_forY(*this,i)
          if ((*this)(0,i)!=current) { res.draw_image(0,j++,get_row(i)); current = (*this)(0,i); }
        res.resize(-100,j,-100,-100,0);
      } break;
      case 'z' : {
        cimg_forZ(*this,i)
          if ((*this)(0,0,i)!=current) { res.draw_image(0,0,j++,get_slice(i)); current = (*this)(0,0,i); }
        res.resize(-100,-100,j,-100,0);
      } break;
      case 'c' : {
        cimg_forC(*this,i)
          if ((*this)(0,0,0,i)!=current) { res.draw_image(0,0,0,j++,get_channel(i)); current = (*this)(0,0,0,i); }
        res.resize(-100,-100,-100,j,0);
      } break;
      default : {
        res.unroll('y');
        cimg_foroff(*this,i)
          if ((*this)[i]!=current) res[j++] = current = (*this)[i];
        res.resize(-100,j,-100,-100,0);
      }
      }
      return res;