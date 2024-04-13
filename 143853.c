     **/
    static CImg<T> rotation_matrix(const float x, const float y, const float z, const float w,
                                   const bool is_quaternion=false) {
      double X, Y, Z, W, N;
      if (is_quaternion) {
        N = std::sqrt((double)x*x + (double)y*y + (double)z*z + (double)w*w);
        if (N>0) { X = x/N; Y = y/N; Z = z/N; W = w/N; }
        else { X = Y = Z = 0; W = 1; }
        return CImg<T>::matrix((T)(X*X + Y*Y - Z*Z - W*W),(T)(2*Y*Z - 2*X*W),(T)(2*X*Z + 2*Y*W),
                               (T)(2*X*W + 2*Y*Z),(T)(X*X - Y*Y + Z*Z - W*W),(T)(2*Z*W - 2*X*Y),
                               (T)(2*Y*W - 2*X*Z),(T)(2*X*Y + 2*Z*W),(T)(X*X - Y*Y - Z*Z + W*W));
      }
      N = cimg::hypot((double)x,(double)y,(double)z);
      if (N>0) { X = x/N; Y = y/N; Z = z/N; }
      else { X = Y = 0; Z = 1; }
      const double ang = w*cimg::PI/180, c = std::cos(ang), omc = 1 - c, s = std::sin(ang);
      return CImg<T>::matrix((T)(X*X*omc + c),(T)(X*Y*omc - Z*s),(T)(X*Z*omc + Y*s),
                             (T)(X*Y*omc + Z*s),(T)(Y*Y*omc + c),(T)(Y*Z*omc - X*s),
                             (T)(X*Z*omc - Y*s),(T)(Y*Z*omc + X*s),(T)(Z*Z*omc + c));