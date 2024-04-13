    template<typename t>
    CImg<t> get_map(const CImg<t>& colormap, const unsigned int boundary_conditions=0) const {
      const ulongT
        whd = (ulongT)_width*_height*_depth, siz = size(),
        cwhd = (ulongT)colormap._width*colormap._height*colormap._depth,
        cwhd2 = 2*cwhd;
      CImg<t> res(_width,_height,_depth,_spectrum*colormap._spectrum);
      switch (colormap._spectrum) {

      case 1 : { // Optimized for scalars
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = ((ulongT)_data[off])%cwhd2;
            res[off] = colormap[ind<cwhd?ind:cwhd2 - ind - 1];
          }
          break;
        case 2 : // Periodic
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = (ulongT)_data[off];
            res[off] = colormap[ind%cwhd];
          }
          break;
        case 1 : // Neumann
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const longT ind = (longT)_data[off];
            res[off] = colormap[cimg::cut(ind,(longT)0,(longT)cwhd - 1)];
          } break;
        default : // Dirichlet
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = (ulongT)_data[off];
            res[off] = ind<cwhd?colormap[ind]:(t)0;
          }
        }
      } break;

      case 2 : { // Optimized for 2D vectors
        const t *const ptrp0 = colormap._data, *const ptrp1 = ptrp0 + cwhd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd2,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              if (ind<cwhd) { ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; }
              else ptrd0[off] = ptrd1[off] = (t)0;
            }
          }
        }
      } break;

      case 3 : { // Optimized for 3D vectors (colors)
        const t *const ptrp0 = colormap._data, *ptrp1 = ptrp0 + cwhd, *ptrp2 = ptrp0 + 2*cwhd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd2,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              if (ind<cwhd) { ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind]; }
              else ptrd0[off] = ptrd1[off] = ptrd2[off] = (t)0;
            }
          }
        }
      } break;

      default : { // Generic version
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,colormap._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &colormap[ind];
              cimg_forC(colormap,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,colormap._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &colormap[ind];
              cimg_forC(colormap,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,colormap._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &colormap[ind];
              cimg_forC(colormap,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,colormap._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              t *const _ptrd = ptrd + off;
              if (ind<cwhd) {
                const t *const ptrp = &colormap[ind];
                cimg_forC(colormap,k) _ptrd[k*whd] = ptrp[k*cwhd];
              } else cimg_forC(colormap,k) _ptrd[k*whd] = (t)0;
            }
          }
        }
      }
      }
      return res;