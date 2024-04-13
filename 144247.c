    template<typename t>
    CImg<_cimg_Tt> operator*(const CImg<t>& img) const {
      typedef _cimg_Ttdouble Ttdouble;
      typedef _cimg_Tt Tt;
      if (_width!=img._height || _depth!=1 || _spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "operator*(): Invalid multiplication of instance by specified "
                                    "matrix (%u,%u,%u,%u,%p)",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);
      CImg<Tt> res(img._width,_height);

      // Check for common cases to optimize.
      if (img._width==1) { // Matrix * Vector
        if (_height==1) switch (_width) { // Vector^T * Vector
          case 1 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0]);
            return res;
          case 2 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1]);
            return res;
          case 3 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2]);
            return res;
          case 4 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2] + (Ttdouble)_data[3]*img[3]);
            return res;
          default : {
            Ttdouble val = 0;
            cimg_forX(*this,i) val+=(Ttdouble)_data[i]*img[i];
            res[0] = val;
            return res;
          }
          } else if (_height==_width) switch (_width) { // Square_matrix * Vector
          case 2 : // 2x2_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1]);
            res[1] = (Tt)((Ttdouble)_data[2]*img[0] + (Ttdouble)_data[3]*img[1]);
            return res;
          case 3 : // 3x3_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2]);
            res[1] = (Tt)((Ttdouble)_data[3]*img[0] + (Ttdouble)_data[4]*img[1] +
                          (Ttdouble)_data[5]*img[2]);
            res[2] = (Tt)((Ttdouble)_data[6]*img[0] + (Ttdouble)_data[7]*img[1] +
                          (Ttdouble)_data[8]*img[2]);
            return res;
          case 4 : // 4x4_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2] + (Ttdouble)_data[3]*img[3]);
            res[1] = (Tt)((Ttdouble)_data[4]*img[0] + (Ttdouble)_data[5]*img[1] +
                          (Ttdouble)_data[6]*img[2] + (Ttdouble)_data[7]*img[3]);
            res[2] = (Tt)((Ttdouble)_data[8]*img[0] + (Ttdouble)_data[9]*img[1] +
                          (Ttdouble)_data[10]*img[2] + (Ttdouble)_data[11]*img[3]);
            res[3] = (Tt)((Ttdouble)_data[12]*img[0] + (Ttdouble)_data[13]*img[1] +
                          (Ttdouble)_data[14]*img[2] + (Ttdouble)_data[15]*img[3]);
            return res;
          }
      } else if (_height==_width) {
        if (img._height==img._width) switch (_width) { // Square_matrix * Square_matrix
          case 2 : // 2x2_matrix * 2x2_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[2]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[3]);
            res[2] = (Tt)((Ttdouble)_data[2]*img[0] + (Ttdouble)_data[3]*img[2]);
            res[3] = (Tt)((Ttdouble)_data[2]*img[1] + (Ttdouble)_data[3]*img[3]);
            return res;
          case 3 : // 3x3_matrix * 3x3_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[3] +
                          (Ttdouble)_data[2]*img[6]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[4] +
                          (Ttdouble)_data[2]*img[7]);
            res[2] = (Tt)((Ttdouble)_data[0]*img[2] + (Ttdouble)_data[1]*img[5] +
                          (Ttdouble)_data[2]*img[8]);
            res[3] = (Tt)((Ttdouble)_data[3]*img[0] + (Ttdouble)_data[4]*img[3] +
                          (Ttdouble)_data[5]*img[6]);
            res[4] = (Tt)((Ttdouble)_data[3]*img[1] + (Ttdouble)_data[4]*img[4] +
                          (Ttdouble)_data[5]*img[7]);
            res[5] = (Tt)((Ttdouble)_data[3]*img[2] + (Ttdouble)_data[4]*img[5] +
                          (Ttdouble)_data[5]*img[8]);
            res[6] = (Tt)((Ttdouble)_data[6]*img[0] + (Ttdouble)_data[7]*img[3] +
                          (Ttdouble)_data[8]*img[6]);
            res[7] = (Tt)((Ttdouble)_data[6]*img[1] + (Ttdouble)_data[7]*img[4] +
                          (Ttdouble)_data[8]*img[7]);
            res[8] = (Tt)((Ttdouble)_data[6]*img[2] + (Ttdouble)_data[7]*img[5] +
                          (Ttdouble)_data[8]*img[8]);
            return res;
          case 4 : // 4x4_matrix * 4x4_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[4] +
                          (Ttdouble)_data[2]*img[8] + (Ttdouble)_data[3]*img[12]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[5] +
                          (Ttdouble)_data[2]*img[9] + (Ttdouble)_data[3]*img[13]);
            res[2] = (Tt)((Ttdouble)_data[0]*img[2] + (Ttdouble)_data[1]*img[6] +
                          (Ttdouble)_data[2]*img[10] + (Ttdouble)_data[3]*img[14]);
            res[3] = (Tt)((Ttdouble)_data[0]*img[3] + (Ttdouble)_data[1]*img[7] +
                          (Ttdouble)_data[2]*img[11] + (Ttdouble)_data[3]*img[15]);
            res[4] = (Tt)((Ttdouble)_data[4]*img[0] + (Ttdouble)_data[5]*img[4] +
                          (Ttdouble)_data[6]*img[8] + (Ttdouble)_data[7]*img[12]);
            res[5] = (Tt)((Ttdouble)_data[4]*img[1] + (Ttdouble)_data[5]*img[5] +
                          (Ttdouble)_data[6]*img[9] + (Ttdouble)_data[7]*img[13]);
            res[6] = (Tt)((Ttdouble)_data[4]*img[2] + (Ttdouble)_data[5]*img[6] +
                          (Ttdouble)_data[6]*img[10] + (Ttdouble)_data[7]*img[14]);
            res[7] = (Tt)((Ttdouble)_data[4]*img[3] + (Ttdouble)_data[5]*img[7] +
                          (Ttdouble)_data[6]*img[11] + (Ttdouble)_data[7]*img[15]);
            res[8] = (Tt)((Ttdouble)_data[8]*img[0] + (Ttdouble)_data[9]*img[4] +
                          (Ttdouble)_data[10]*img[8] + (Ttdouble)_data[11]*img[12]);
            res[9] = (Tt)((Ttdouble)_data[8]*img[1] + (Ttdouble)_data[9]*img[5] +
                          (Ttdouble)_data[10]*img[9] + (Ttdouble)_data[11]*img[13]);
            res[10] = (Tt)((Ttdouble)_data[8]*img[2] + (Ttdouble)_data[9]*img[6] +
                           (Ttdouble)_data[10]*img[10] + (Ttdouble)_data[11]*img[14]);
            res[11] = (Tt)((Ttdouble)_data[8]*img[3] + (Ttdouble)_data[9]*img[7] +
                           (Ttdouble)_data[10]*img[11] + (Ttdouble)_data[11]*img[15]);
            res[12] = (Tt)((Ttdouble)_data[12]*img[0] + (Ttdouble)_data[13]*img[4] +
                           (Ttdouble)_data[14]*img[8] + (Ttdouble)_data[15]*img[12]);
            res[13] = (Tt)((Ttdouble)_data[12]*img[1] + (Ttdouble)_data[13]*img[5] +
                           (Ttdouble)_data[14]*img[9] + (Ttdouble)_data[15]*img[13]);
            res[14] = (Tt)((Ttdouble)_data[12]*img[2] + (Ttdouble)_data[13]*img[6] +
                           (Ttdouble)_data[14]*img[10] + (Ttdouble)_data[15]*img[14]);
            res[15] = (Tt)((Ttdouble)_data[12]*img[3] + (Ttdouble)_data[13]*img[7] +
                           (Ttdouble)_data[14]*img[11] + (Ttdouble)_data[15]*img[15]);
            return res;
          } else switch (_width) { // Square_matrix * Matrix
          case 2 : { // 2x2_matrix * Matrix
            const t *ps0 = img.data(), *ps1 = img.data(0,1);
            Tt *pd0 = res.data(), *pd1 = res.data(0,1);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1],
              a2 = (Ttdouble)_data[2], a3 = (Ttdouble)_data[3];
            cimg_forX(img,i) {
              const Ttdouble x = (Ttdouble)*(ps0++), y = (Ttdouble)*(ps1++);
              *(pd0++) = (Tt)(a0*x + a1*y);
              *(pd1++) = (Tt)(a2*x + a3*y);
            }
            return res;
          }
          case 3 : { // 3x3_matrix * Matrix
            const t *ps0 = img.data(), *ps1 = img.data(0,1), *ps2 = img.data(0,2);
            Tt *pd0 = res.data(), *pd1 = res.data(0,1), *pd2 = res.data(0,2);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1], a2 = (Ttdouble)_data[2],
              a3 = (Ttdouble)_data[3], a4 = (Ttdouble)_data[4], a5 = (Ttdouble)_data[5],
              a6 = (Ttdouble)_data[6], a7 = (Ttdouble)_data[7], a8 = (Ttdouble)_data[8];
            cimg_forX(img,i) {
              const Ttdouble x = (Ttdouble)*(ps0++), y = (Ttdouble)*(ps1++), z = (Ttdouble)*(ps2++);
              *(pd0++) = (Tt)(a0*x + a1*y + a2*z);
              *(pd1++) = (Tt)(a3*x + a4*y + a5*z);
              *(pd2++) = (Tt)(a6*x + a7*y + a8*z);
            }
            return res;
          }
          case 4 : { // 4x4_matrix * Matrix
            const t *ps0 = img.data(), *ps1 = img.data(0,1), *ps2 = img.data(0,2), *ps3 = img.data(0,3);
            Tt *pd0 = res.data(), *pd1 = res.data(0,1), *pd2 = res.data(0,2), *pd3 = res.data(0,3);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1], a2 = (Ttdouble)_data[2], a3 = (Ttdouble)_data[3],
              a4 = (Ttdouble)_data[4], a5 = (Ttdouble)_data[5], a6 = (Ttdouble)_data[6], a7 = (Ttdouble)_data[7],
              a8 = (Ttdouble)_data[8], a9 = (Ttdouble)_data[9], a10 = (Ttdouble)_data[10], a11 = (Ttdouble)_data[11],
              a12 = (Ttdouble)_data[12], a13 = (Ttdouble)_data[13], a14 = (Ttdouble)_data[14],
              a15 = (Ttdouble)_data[15];
            cimg_forX(img,col) {
              const Ttdouble x = (Ttdouble)*(ps0++), y = (Ttdouble)*(ps1++), z = (Ttdouble)*(ps2++),
                c = (Ttdouble)*(ps3++);
              *(pd0++) = (Tt)(a0*x + a1*y + a2*z + a3*c);
              *(pd1++) = (Tt)(a4*x + a5*y + a6*z + a7*c);
              *(pd2++) = (Tt)(a8*x + a9*y + a10*z + a11*c);
              *(pd3++) = (Tt)(a12*x + a13*y + a14*z + a15*c);
            }
            return res;
          }
          }
      }

      // Fallback to generic version.
#if cimg_use_openmp!=0
      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                         cimg_openmp_if(size()>(cimg_openmp_sizefactor)*1024 &&
                                        img.size()>(cimg_openmp_sizefactor)*1024))
        cimg_forXY(res,i,j) {
          Ttdouble value = 0; cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k); res(i,j) = (Tt)value;
      }
#else
      Tt *ptrd = res._data;
      cimg_forXY(res,i,j) {
        Ttdouble value = 0; cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k); *(ptrd++) = (Tt)value;
      }
#endif
      return res;