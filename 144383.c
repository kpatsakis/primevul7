    **/
    CImg<T>& set_linear_atXYZ(const T& value, const float fx, const float fy=0, const float fz=0, const int c=0,
                              const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z;
      if (c>=0 && c<spectrum()) {
        if (z>=0 && z<depth()) {
          if (y>=0 && y<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
            }
          }
          if (ny>=0 && ny<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*dy*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*dy*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
            }
          }
        }
        if (nz>=0 && nz<depth()) {
          if (y>=0 && y<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
              (*this)(x,y,nz,c) = (T)(w1*value + w2*(*this)(x,y,nz,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
              (*this)(nx,y,nz,c) = (T)(w1*value + w2*(*this)(nx,y,nz,c));
            }
          }
          if (ny>=0 && ny<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*dy*dz, w2 = is_added?1:(1 - w1);
              (*this)(x,ny,nz,c) = (T)(w1*value + w2*(*this)(x,ny,nz,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*dy*dz, w2 = is_added?1:(1 - w1);
              (*this)(nx,ny,nz,c) = (T)(w1*value + w2*(*this)(nx,ny,nz,c));
            }
          }
        }
      }
      return *this;