     **/
    double det() const {
      if (is_empty() || _width!=_height || _depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "det(): Instance is not a square matrix.",
                                    cimg_instance);

      switch (_width) {
      case 1 : return (double)((*this)(0,0));
      case 2 : return (double)((*this)(0,0))*(double)((*this)(1,1)) - (double)((*this)(0,1))*(double)((*this)(1,0));
      case 3 : {
        const double
          a = (double)_data[0], d = (double)_data[1], g = (double)_data[2],
          b = (double)_data[3], e = (double)_data[4], h = (double)_data[5],
          c = (double)_data[6], f = (double)_data[7], i = (double)_data[8];
        return i*a*e - a*h*f - i*b*d + b*g*f + c*d*h - c*g*e;
      }
      default : {
        CImg<Tfloat> lu(*this,false);
        CImg<uintT> indx;
        bool d;
        lu._LU(indx,d);
        double res = d?(double)1:(double)-1;
        cimg_forX(lu,i) res*=lu(i,i);
        return res;
      }
      }