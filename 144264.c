    **/
    CImg<T>& set_linear_atXY(const T& value, const float fx, const float fy=0, const int z=0, const int c=0,
                             const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1;
      const float
        dx = fx - x,
        dy = fy - y;
      if (z>=0 && z<depth() && c>=0 && c<spectrum()) {
        if (y>=0 && y<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*(1 - dy), w2 = is_added?1:(1 - w1);
            (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*(1 - dy), w2 = is_added?1:(1 - w1);
            (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
          }
        }
        if (ny>=0 && ny<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*dy, w2 = is_added?1:(1 - w1);
            (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*dy, w2 = is_added?1:(1 - w1);
            (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
          }
        }
      }
      return *this;