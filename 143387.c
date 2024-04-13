    //! Resize image to half-size along XY axes, using an optimized filter \newinstance.
    CImg<T> get_resize_halfXY() const {
      if (is_empty()) return *this;
      static const Tfloat kernel[9] = { 0.07842776544f, 0.1231940459f, 0.07842776544f,
                                        0.1231940459f,  0.1935127547f, 0.1231940459f,
                                        0.07842776544f, 0.1231940459f, 0.07842776544f };
      CImg<T> I(9), res(_width/2,_height/2,_depth,_spectrum);
      T *ptrd = res._data;
      cimg_forZC(*this,z,c) cimg_for3x3(*this,x,y,z,c,I,T)
        if (x%2 && y%2) *(ptrd++) = (T)
                          (I[0]*kernel[0] + I[1]*kernel[1] + I[2]*kernel[2] +
                           I[3]*kernel[3] + I[4]*kernel[4] + I[5]*kernel[5] +
                           I[6]*kernel[6] + I[7]*kernel[7] + I[8]*kernel[8]);
      return res;