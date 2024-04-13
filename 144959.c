    **/
    CImg<T>& set_linear_atX(const T& value, const float fx, const int y=0, const int z=0, const int c=0,
                            const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1;
      const float
        dx = fx - x;
      if (y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum()) {
        if (x>=0 && x<width()) {
          const float w1 = 1 - dx, w2 = is_added?1:(1 - w1);
          (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
        }
        if (nx>=0 && nx<width()) {
          const float w1 = dx, w2 = is_added?1:(1 - w1);
          (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
        }
      }
      return *this;