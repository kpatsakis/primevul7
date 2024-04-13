    template<typename t>
    CImg<T> get_warp(const CImg<t>& p_warp, const unsigned int mode=0,
                     const unsigned int interpolation=1, const unsigned int boundary_conditions=0) const {
      if (is_empty() || !p_warp) return *this;
      if (mode && !is_sameXYZ(p_warp))
        throw CImgArgumentException(_cimg_instance
                                    "warp(): Instance and specified relative warping field (%u,%u,%u,%u,%p) "
                                    "have different XYZ dimensions.",
                                    cimg_instance,
                                    p_warp._width,p_warp._height,p_warp._depth,p_warp._spectrum,p_warp._data);

      CImg<T> res(p_warp._width,p_warp._height,p_warp._depth,_spectrum);

      if (p_warp._spectrum==1) { // 1D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atX(*(ptrs++),x + (float)*(ptrs0++),y,z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = x + (int)cimg::round(*(ptrs0++));
                if (X>=0 && X<width()) res(X,y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atX(*(ptrs++),(float)*(ptrs0++),y,z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = (int)cimg::round(*(ptrs0++));
                if (X>=0 && X<width()) res(X,y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod(x - (float)*(ptrs0++),w2);
                  *(ptrd++) = _cubic_atX_c(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_pc(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_c(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atX_c(x - (float)*(ptrs0++),y,z,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod(x - (float)*(ptrs0++),w2);
                  *(ptrd++) = (T)_linear_atX(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX_p(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atX(x - (float)*(ptrs0++),y,z,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atX(x - (int)cimg::round(*(ptrs0++)),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atX(x - (int)cimg::round(*(ptrs0++)),y,z,c,(T)0);
              }
            }
        }
        else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
                cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod((float)*(ptrs0++),w2);
                  *(ptrd++) = _cubic_atX_c(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_pc((float)*(ptrs0++),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_c((float)*(ptrs0++),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atX_c((float)*(ptrs0++),0,0,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
                cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod((float)*(ptrs0++),w2);
                  *(ptrd++) = (T)_linear_atX(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX_p((float)*(ptrs0++),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX((float)*(ptrs0++),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atX((float)*(ptrs0++),0,0,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
                cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atX((int)cimg::round(*(ptrs0++)),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atX((int)cimg::round(*(ptrs0++)),0,0,c,(T)0);
              }
            }
        }

      } else if (p_warp._spectrum==2) { // 2D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXY(*(ptrs++),x + (float)*(ptrs0++),y + (float)*(ptrs1++),z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = x + (int)cimg::round(*(ptrs0++)), Y = y + (int)cimg::round(*(ptrs1++));
                if (X>=0 && X<width() && Y>=0 && Y<height()) res(X,Y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXY(*(ptrs++),(float)*(ptrs0++),(float)*(ptrs1++),z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = (int)cimg::round(*(ptrs0++)), Y = (int)cimg::round(*(ptrs1++));
                if (X>=0 && X<width() && Y>=0 && Y<height()) res(X,Y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2);
                  *(ptrd++) = _cubic_atXY_c(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_pc(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXY_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2);
                  *(ptrd++) = (T)_linear_atXY(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY_p(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXY(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod(y - (int)cimg::round(*(ptrs1++)),h2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod(y - (int)cimg::round(*(ptrs1++)),height()),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXY(x - (int)cimg::round(*(ptrs0++)),
                                                   y - (int)cimg::round(*(ptrs1++)),z,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXY(x - (int)cimg::round(*(ptrs0++)),
                                                  y - (int)cimg::round(*(ptrs1++)),z,c,(T)0);
              }
            }
        } else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2);
                  *(ptrd++) = _cubic_atXY_c(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_pc((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_c((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXY_c((float)*(ptrs0++),(float)*(ptrs1++),0,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2);
                  *(ptrd++) = (T)_linear_atXY(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY_p((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXY((float)*(ptrs0++),(float)*(ptrs1++),0,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod((int)cimg::round(*(ptrs1++)),h2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod((int)cimg::round(*(ptrs1++)),height()),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXY((int)cimg::round(*(ptrs0++)),
                                                   (int)cimg::round(*(ptrs1++)),0,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXY((int)cimg::round(*(ptrs0++)),
                                                  (int)cimg::round(*(ptrs1++)),0,c,(T)0);
              }
            }
        }

      } else { // 3D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXYZ(*(ptrs++),x + (float)*(ptrs0++),y + (float)*(ptrs1++),
                                                    z + (float)*(ptrs2++),c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int
                  X = x + (int)cimg::round(*(ptrs0++)),
                  Y = y + (int)cimg::round(*(ptrs1++)),
                  Z = z + (int)cimg::round(*(ptrs2++));
                if (X>=0 && X<width() && Y>=0 && Y<height() && Z>=0 && Z<depth()) res(X,Y,Z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXYZ(*(ptrs++),(float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int
                  X = (int)cimg::round(*(ptrs0++)),
                  Y = (int)cimg::round(*(ptrs1++)),
                  Z = (int)cimg::round(*(ptrs2++));
                if (X>=0 && X<width() && Y>=0 && Y<height() && Z>=0 && Z<depth()) res(X,Y,Z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2),
                    mz = cimg::mod(z - (float)*(ptrs2++),d2);
                  *(ptrd++) = _cubic_atXYZ_c(mx<width()?mx:w2 - mx - 1,
                                             my<height()?my:h2 - my - 1,
                                             mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = _cubic_atXYZ_pc(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = _cubic_atXYZ_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = cubic_atXYZ_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2),
                    mz = cimg::mod(z - (float)*(ptrs2++),d2);
                  *(ptrd++) = (T)_linear_atXYZ(mx<width()?mx:w2 - mx - 1,
                                               my<height()?my:h2 - my - 1,
                                               mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ_p(x - (float)*(ptrs0++),y - (float)*(ptrs1++),
                                                                z - (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = (T)_linear_atXYZ(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = (T)linear_atXYZ(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c,(T)0);
              }
            }
          else // Nearest neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod(y - (int)cimg::round(*(ptrs1++)),h2),
                    mz = cimg::mod(z - (int)cimg::round(*(ptrs2++)),d2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,
                                      my<height()?my:h2 - my - 1,
                                      mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod(y - (int)cimg::round(*(ptrs1++)),height()),
                                                     cimg::mod(z - (int)cimg::round(*(ptrs2++)),depth()),c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXYZ(x - (int)cimg::round(*(ptrs0++)),
                                                    y - (int)cimg::round(*(ptrs1++)),
                                                    z - (int)cimg::round(*(ptrs2++)),c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXYZ(x - (int)cimg::round(*(ptrs0++)),
                                                   y - (int)cimg::round(*(ptrs1++)),
                                                   z - (int)cimg::round(*(ptrs2++)),c,(T)0);
              }
            }
        } else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2),
                    mz = cimg::mod((float)*(ptrs2++),d2);
                  *(ptrd++) = _cubic_atXYZ_c(mx<width()?mx:w2 - mx - 1,
                                             my<height()?my:h2 - my - 1,
                                             mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXYZ_pc((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXYZ_c((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXYZ_c((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),
                                                           c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2),
                    mz = cimg::mod((float)*(ptrs2++),d2);
                  *(ptrd++) = (T)_linear_atXYZ(mx<width()?mx:w2 - mx - 1,
                                               my<height()?my:h2 - my - 1,
                                               mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 :// Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ_p((float)*(ptrs0++),(float)*(ptrs1++),
                                                                (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXYZ((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),
                                                             c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod((int)cimg::round(*(ptrs1++)),h2),
                    mz = cimg::mod((int)cimg::round(*(ptrs2++)),d2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,
                                      my<height()?my:h2 - my - 1,
                                      mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod((int)cimg::round(*(ptrs1++)),height()),
                                                     cimg::mod((int)cimg::round(*(ptrs2++)),depth()),c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXYZ((int)cimg::round(*(ptrs0++)),
                                                    (int)cimg::round(*(ptrs1++)),
                                                    (int)cimg::round(*(ptrs2++)),c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXYZ((int)cimg::round(*(ptrs0++)),
                                                   (int)cimg::round(*(ptrs1++)),
                                                   (int)cimg::round(*(ptrs2++)),c,(T)0);
              }
            }
        }
      }
      return res;