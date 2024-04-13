    template<typename t>
    CImg<_cimg_Ttfloat> _correlate(const CImg<t>& kernel, const bool boundary_conditions,
                                   const bool is_normalized,
/*                                   const int x0, const int y0, const int z0,
                                   const int x1, const int y1, const int z1,
                                   const int xstep, const int ystep, const int zstep,
*/
                                   const int dilation,
                                   const bool is_convolution) const {
      if (is_empty() || !kernel || dilation<0) return *this;
      typedef _cimg_Ttfloat Ttfloat;
      CImg<Ttfloat> res;
      const ulongT
        res_whd = (ulongT)_width*_height*_depth,
        res_size = res_whd*std::max(_spectrum,kernel._spectrum);
      const bool
        is_inner_parallel = _width*_height*_depth>=(cimg_openmp_sizefactor)*32768,
        is_outer_parallel = res_size>=(cimg_openmp_sizefactor)*32768;
      _cimg_abort_init_omp;
      cimg_abort_init;

      if (kernel._width==kernel._height &&
          ((kernel._depth==1 && kernel._width<=5) || (kernel._depth==kernel._width && kernel._width<=3))) {

        // Special optimization done for 2x2, 3x3, 4x4, 5x5, 6x6, 2x2x2 and 3x3x3 kernel.
        if (!boundary_conditions && res_whd<=3000*3000) { // Dirichlet boundaries
          // For relatively small images, adding a zero border then use optimized NxN convolution loops is faster.
          res = (kernel._depth==1?get_crop(-1,-1,_width,_height):get_crop(-1,-1,-1,_width,_height,_depth)).
            _correlate(kernel,true,is_normalized,dilation,is_convolution);
          if (kernel._depth==1) res.crop(1,1,res._width - 2,res._height - 2);
          else res.crop(1,1,1,res._width - 2,res._height - 2,res._depth - 2);

        } else { // Neumann boundaries
          res.assign(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
          cimg::unused(is_inner_parallel,is_outer_parallel);
          CImg<t> _kernel;
          if (is_convolution) { // Add empty column/row/slice to shift kernel center in case of convolution
            const int dw = !(kernel.width()%2), dh = !(kernel.height()%2), dd = !(kernel.depth()%2);
            if (dw || dh || dd)
              kernel.get_resize(kernel.width() + dw,kernel.height() + dh,kernel.depth() + dd,-100,0,0).
                move_to(_kernel);
          }
          if (!_kernel) _kernel = kernel.get_shared();

          switch (_kernel._depth) {
          case 3 : { // 3x3x3 kernel
            cimg_forC(res,c) {
              cimg_abort_test;
              const CImg<T> I = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
              CImg<T> _res = res.get_shared_channel(c);
              if (is_normalized) {
                const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                  cimg_forXYZ(res,x,y,z) {
                  const int
                    px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                    py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                    pz = z>dilation?z - dilation:z, nz = z + dilation<res.depth()?z + dilation:z;
                  const Ttfloat N = M2*(cimg::sqr(I(px,py,pz)) + cimg::sqr(I(x,py,pz)) + cimg::sqr(I(nx,py,pz)) +
                                        cimg::sqr(I(px,y,pz)) + cimg::sqr(I(x,y,pz)) + cimg::sqr(I(nx,y,pz)) +
                                        cimg::sqr(I(px,ny,pz)) + cimg::sqr(I(x,ny,pz)) + cimg::sqr(I(nx,ny,pz)) +
                                        cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) + cimg::sqr(I(nx,py,z)) +
                                        cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                        cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)) +
                                        cimg::sqr(I(px,py,nz)) + cimg::sqr(I(x,py,nz)) + cimg::sqr(I(nx,py,nz)) +
                                        cimg::sqr(I(px,y,nz)) + cimg::sqr(I(x,y,nz)) + cimg::sqr(I(nx,y,nz)) +
                                        cimg::sqr(I(px,ny,nz)) + cimg::sqr(I(x,ny,nz)) + cimg::sqr(I(nx,ny,nz)));
                  _res(x,y,z) = (Ttfloat)(N?(K[0]*I(px,py,pz) + K[1]*I(x,py,pz) + K[2]*I(nx,py,pz) +
                                             K[3]*I(px,y,pz) + K[4]*I(x,y,pz) + K[5]*I(nx,y,pz) +
                                             K[6]*I(px,ny,pz) + K[7]*I(x,ny,pz) + K[8]*I(nx,ny,pz) +
                                             K[9]*I(px,py,z) + K[10]*I(x,py,z) + K[11]*I(nx,py,z) +
                                             K[12]*I(px,y,z) + K[13]*I(x,y,z) + K[14]*I(nx,y,z) +
                                             K[15]*I(px,ny,z) + K[16]*I(x,ny,z) + K[17]*I(nx,ny,z) +
                                             K[18]*I(px,py,nz) + K[19]*I(x,py,nz) + K[20]*I(nx,py,nz) +
                                             K[21]*I(px,y,nz) + K[22]*I(x,y,nz) + K[23]*I(nx,y,nz) +
                                             K[24]*I(px,ny,nz) + K[25]*I(x,ny,nz) + K[26]*I(nx,ny,nz))/std::sqrt(N):0);
                }
              } else {
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                  cimg_forXYZ(res,x,y,z) {
                  const int
                    px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                    py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                    pz = z>dilation?z - dilation:z, nz = z + dilation<res.depth()?z + dilation:z;
                  _res(x,y,z) = (Ttfloat)(K[0]*I(px,py,pz) + K[1]*I(x,py,pz) + K[2]*I(nx,py,pz) +
                                          K[3]*I(px,y,pz) + K[4]*I(x,y,pz) + K[5]*I(nx,y,pz) +
                                          K[6]*I(px,ny,pz) + K[7]*I(x,ny,pz) + K[8]*I(nx,ny,pz) +
                                          K[9]*I(px,py,z) + K[10]*I(x,py,z) + K[11]*I(nx,py,z) +
                                          K[12]*I(px,y,z) + K[13]*I(x,y,z) + K[14]*I(nx,y,z) +
                                          K[15]*I(px,ny,z) + K[16]*I(x,ny,z) + K[17]*I(nx,ny,z) +
                                          K[18]*I(px,py,nz) + K[19]*I(x,py,nz) + K[20]*I(nx,py,nz) +
                                          K[21]*I(px,y,nz) + K[22]*I(x,y,nz) + K[23]*I(nx,y,nz) +
                                          K[24]*I(px,ny,nz) + K[25]*I(x,ny,nz) + K[26]*I(nx,ny,nz));
                }
              }
            }
          } break;

          case 2 : { // 2x2x2 kernel
            cimg_forC(res,c) {
              cimg_abort_test;
              const CImg<T> I = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
              CImg<T> _res = res.get_shared_channel(c);
              if (is_normalized) {
                const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                  cimg_forXYZ(res,x,y,z) {
                  const int nx = x + 1<res.width()?x + 1:x, ny = y + 1<res.height()?y + 1:y,
                            nz = z + 1<res.depth()?z + 1:z;
                  const Ttfloat N = M2*(cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                        cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)) +
                                        cimg::sqr(I(x,y,nz)) + cimg::sqr(I(nx,y,nz)) +
                                        cimg::sqr(I(x,ny,nz)) + cimg::sqr(I(nx,ny,nz)));
                  _res(x,y,z) = (Ttfloat)(N?(K[0]*I(x,y,z) + K[1]*I(nx,y,z) +
                                             K[2]*I(x,ny,z) + K[3]*I(nx,ny,z) +
                                             K[4]*I(x,y,nz) + K[5]*I(nx,y,nz) +
                                             K[6]*I(x,ny,nz) + K[7]*I(nx,ny,nz))/std::sqrt(N):0);
                }
              } else {
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                  cimg_forXYZ(res,x,y,z) {
                  const int nx = x + 1<res.width()?x + 1:x, ny = y + 1<res.height()?y + 1:y,
                            nz = z + 1<res.depth()?z + 1:z;
                  _res(x,y,z) = (Ttfloat)(K[0]*I(x,y,z) + K[1]*I(nx,y,z) +
                                          K[2]*I(x,ny,z) + K[3]*I(nx,ny,z) +
                                          K[4]*I(x,y,nz) + K[5]*I(nx,y,nz) +
                                          K[6]*I(x,ny,nz) + K[7]*I(nx,ny,nz));
                }
              }
            }
          } break;

          default :
          case 1 :
            switch (_kernel._width) {
            case 5 : { // 5x5 kernel
              cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> I = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> _res = res.get_shared_channel(c);
                if (is_normalized) {
                  const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      bx = px>dilation?px - dilation:px, ax = nx + dilation<res.width()?nx + dilation:nx,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                      by = py>dilation?py - dilation:py, ay = ny + dilation<res.height()?ny + dilation:ny;
                    const Ttfloat N = M2*(cimg::sqr(I(bx,by,z)) + cimg::sqr(I(px,by,z)) + cimg::sqr(I(x,by,z)) +
                                          cimg::sqr(I(nx,by,z)) + cimg::sqr(I(ax,by,z)) +
                                          cimg::sqr(I(bx,py,z)) + cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) +
                                          cimg::sqr(I(nx,py,z)) + cimg::sqr(I(ax,py,z)) +
                                          cimg::sqr(I(bx,y,z)) + cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) +
                                          cimg::sqr(I(nx,y,z)) + cimg::sqr(I(ax,y,z)) +
                                          cimg::sqr(I(bx,ny,z)) + cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) +
                                          cimg::sqr(I(nx,ny,z)) + cimg::sqr(I(ax,ny,z)) +
                                          cimg::sqr(I(bx,ay,z)) + cimg::sqr(I(px,ay,z)) + cimg::sqr(I(x,ay,z)) +
                                          cimg::sqr(I(nx,ay,z)) + cimg::sqr(I(ax,ay,z)));
                    _res(x,y,z) = (Ttfloat)(N?(K[0]*I(bx,by,z) + K[1]*I(px,by,z) + K[2]*I(x,by,z) +
                                               K[3]*I(nx,by,z) + K[4]*I(ax,by,z) +
                                               K[5]*I(bx,py,z) + K[6]*I(px,py,z) + K[7]*I(x,py,z) +
                                               K[8]*I(nx,py,z) + K[9]*I(ax,py,z) +
                                               K[10]*I(bx,y,z) + K[11]*I(px,y,z) + K[12]*I(x,y,z) +
                                               K[13]*I(nx,y,z) + K[14]*I(ax,y,z) +
                                               K[15]*I(bx,ny,z) + K[16]*I(px,ny,z) + K[17]*I(x,ny,z) +
                                               K[18]*I(nx,ny,z) + K[19]*I(ax,ny,z) +
                                               K[20]*I(bx,ay,z) + K[21]*I(px,ay,z) + K[22]*I(x,ay,z) +
                                               K[23]*I(nx,ay,z) + K[24]*I(ax,ay,z))/std::sqrt(N):0);
                  }
                } else {
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      bx = px>dilation?px - dilation:px, ax = nx + dilation<res.width()?nx + dilation:nx,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                      by = py>dilation?py - dilation:py, ay = ny + dilation<res.height()?ny + dilation:ny;
                    _res(x,y,z) = (Ttfloat)(K[0]*I(bx,by,z) + K[1]*I(px,by,z) + K[2]*I(x,by,z) +
                                            K[3]*I(nx,by,z) + K[4]*I(ax,by,z) +
                                            K[5]*I(bx,py,z) + K[6]*I(px,py,z) + K[7]*I(x,py,z) +
                                            K[8]*I(nx,py,z) + K[9]*I(ax,py,z) +
                                            K[10]*I(bx,y,z) + K[11]*I(px,y,z) + K[12]*I(x,y,z) +
                                            K[13]*I(nx,y,z) + K[14]*I(ax,y,z) +
                                            K[15]*I(bx,ny,z) + K[16]*I(px,ny,z) + K[17]*I(x,ny,z) +
                                            K[18]*I(nx,ny,z) + K[19]*I(ax,ny,z) +
                                            K[20]*I(bx,ay,z) + K[21]*I(px,ay,z) + K[22]*I(x,ay,z) +
                                            K[23]*I(nx,ay,z) + K[24]*I(ax,ay,z));
                  }
                }
              }
            } break;

            case 4 : { // 4x4 kernel
              cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> I = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> _res = res.get_shared_channel(c);
                if (is_normalized) {
                  const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      ax = nx + dilation<res.width()?nx + dilation:nx,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                      ay = ny + dilation<res.height()?ny + dilation:ny;
                    const Ttfloat N = M2*(cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) +
                                          cimg::sqr(I(nx,py,z)) + cimg::sqr(I(ax,py,z)) +
                                          cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) +
                                          cimg::sqr(I(nx,y,z)) + cimg::sqr(I(ax,y,z)) +
                                          cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) +
                                          cimg::sqr(I(nx,ny,z)) + cimg::sqr(I(ax,ny,z)) +
                                          cimg::sqr(I(px,ay,z)) + cimg::sqr(I(x,ay,z)) +
                                          cimg::sqr(I(nx,ay,z)) + cimg::sqr(I(ax,ay,z)));
                    _res(x,y,z) = (Ttfloat)(N?(K[0]*I(px,py,z) + K[1]*I(x,py,z) +
                                               K[2]*I(nx,py,z) + K[3]*I(ax,py,z) +
                                               K[4]*I(px,y,z) + K[5]*I(x,y,z) +
                                               K[6]*I(nx,y,z) + K[7]*I(ax,y,z) +
                                               K[8]*I(px,ny,z) + K[9]*I(x,ny,z) +
                                               K[10]*I(nx,ny,z) + K[11]*I(ax,ny,z) +
                                               K[12]*I(px,ay,z) + K[13]*I(x,ay,z) +
                                               K[14]*I(nx,ay,z) + K[15]*I(ax,ay,z))/std::sqrt(N):0);
                  }
                } else {
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      ax = nx + dilation<res.width()?nx + dilation:nx,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y,
                      ay = ny + dilation<res.height()?ny + dilation:ny;
                    _res(x,y,z) = (Ttfloat)(K[0]*I(px,py,z) + K[1]*I(x,py,z) +
                                            K[2]*I(nx,py,z) + K[3]*I(ax,py,z) +
                                            K[4]*I(px,y,z) + K[5]*I(x,y,z) +
                                            K[6]*I(nx,y,z) + K[7]*I(ax,y,z) +
                                            K[8]*I(px,ny,z) + K[9]*I(x,ny,z) +
                                            K[10]*I(nx,ny,z) + K[11]*I(ax,ny,z) +
                                            K[12]*I(px,ay,z) + K[13]*I(x,ay,z) +
                                            K[14]*I(nx,ay,z) + K[15]*I(ax,ay,z));
                  }
                }
              }
            } break;

            case 3 : { // 3x3 kernel
              cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> I = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> _res = res.get_shared_channel(c);
                if (is_normalized) {
                  const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y;
                    const Ttfloat N = M2*(cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) + cimg::sqr(I(nx,py,z)) +
                                          cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                          cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)));
                    _res(x,y,z) = (Ttfloat)(N?(K[0]*I(px,py,z) + K[1]*I(x,py,z) + K[2]*I(nx,py,z) +
                                               K[3]*I(px,y,z) + K[4]*I(x,y,z) + K[5]*I(nx,y,z) +
                                               K[6]*I(px,ny,z) + K[7]*I(x,ny,z) + K[8]*I(nx,ny,z))/std::sqrt(N):0);
                  }
                } else {
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int
                      px = x>dilation?x - dilation:x, nx = x + dilation<res.width()?x + dilation:x,
                      py = y>dilation?y - dilation:y, ny = y + dilation<res.height()?y + dilation:y;
                    _res(x,y,z) = (Ttfloat)(K[0]*I(px,py,z) + K[1]*I(x,py,z) + K[2]*I(nx,py,z) +
                                            K[3]*I(px,y,z) + K[4]*I(x,y,z) + K[5]*I(nx,y,z) +
                                            K[6]*I(px,ny,z) + K[7]*I(x,ny,z) + K[8]*I(nx,ny,z));
                  }
                }
              }
            } break;

            case 2 : { // 2x2 kernel
              cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> I = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> _res = res.get_shared_channel(c);
                if (is_normalized) {
                  const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int nx = x + 1<res.width()?x + 1:x, ny = y + 1<res.height()?y + 1:y;
                    const Ttfloat N = M2*(cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                          cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)));
                    _res(x,y,z) = (Ttfloat)(N?(K[0]*I(x,y,z) + K[1]*I(nx,y,z) +
                                               K[2]*I(x,ny,z) + K[3]*I(nx,ny,z))/std::sqrt(N):0);
                  }
                } else {
                  cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_res.size(),16384))
                    cimg_forXYZ(res,x,y,z) {
                    const int nx = x + 1<res.width()?x + 1:x, ny = y + 1<res.height()?y + 1:y;
                    _res(x,y,z) = (Ttfloat)(K[0]*I(x,y,z) + K[1]*I(nx,y,z) +
                                            K[2]*I(x,ny,z) + K[3]*I(nx,ny,z));
                  }
                }
              }
            } break;

            default :
            case 1 : // 1x1 kernel
              if (is_normalized) res.fill(1);
              else cimg_forC(res,c) {
                  cimg_abort_test;
                  const CImg<T> img = get_shared_channel(c%_spectrum);
                  res.get_shared_channel(c).assign(img)*=_kernel(0,0,0,c%kernel._spectrum);
                }
              break;
            }
          }
        }
      } else { // Generic version for other kernels and boundary conditions
        res.assign(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
        int
          mx2 = kernel.width()/2, my2 = kernel.height()/2, mz2 = kernel.depth()/2,
          mx1 = kernel.width() - mx2 - 1, my1 = kernel.height() - my2 - 1, mz1 = kernel.depth() - mz2 - 1;
        if (is_convolution) cimg::swap(mx1,mx2,my1,my2,mz1,mz2); // Shift kernel center in case of convolution
        const int
          mxs = dilation*mx1, mys = dilation*my1, mzs = dilation*mz1,
          mxe = width() - dilation*mx2, mye = height() - dilation*my2, mze = depth() - dilation*mz2;
        cimg_pragma_openmp(parallel for cimg_openmp_if(!is_inner_parallel && is_outer_parallel))
        cimg_forC(res,c) _cimg_abort_try_omp {
          cimg_abort_test;
          const CImg<T> img = get_shared_channel(c%_spectrum);
          const CImg<t> K = kernel.get_shared_channel(c%kernel._spectrum);
          if (is_normalized) { // Normalized correlation
            const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
            for (int z = mzs; z<mze; ++z)
              for (int y = mys; y<mye; ++y)
                for (int x = mxs; x<mxe; ++x) _cimg_abort_try_omp2 {
                  cimg_abort_test2;
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img(x + dilation*xm,y + dilation*ym,z + dilation*zm);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M2;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                } _cimg_abort_catch_omp2
            if (boundary_conditions)
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<mys || y>=mye || z<mzs || z>=mze)?++x:((x<mxs - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img._atXYZ(x + dilation*xm,y + dilation*ym,z + dilation*zm);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M2;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                }
              } _cimg_abort_catch_omp2
            else
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<mys || y>=mye || z<mzs || z>=mze)?++x:((x<mxs - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img.atXYZ(x + dilation*xm,y + dilation*ym,z + dilation*zm,0,(T)0);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M2;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                }
              } _cimg_abort_catch_omp2
          } else { // Classical correlation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
            for (int z = mzs; z<mze; ++z)
              for (int y = mys; y<mye; ++y)
                for (int x = mxs; x<mxe; ++x) _cimg_abort_try_omp2 {
                  cimg_abort_test2;
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img(x + dilation*xm,y + dilation*ym,z + dilation*zm)*K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                } _cimg_abort_catch_omp2
            if (boundary_conditions)
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<mys || y>=mye || z<mzs || z>=mze)?++x:((x<mxs - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img._atXYZ(x + dilation*xm,y + dilation*ym,z + dilation*zm)*K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                }
              } _cimg_abort_catch_omp2
            else
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<mys || y>=mye || z<mzs || z>=mze)?++x:((x<mxs - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img.atXYZ(x + dilation*xm,y + dilation*ym,z + dilation*zm,0,(T)0)*
                          K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                }
              } _cimg_abort_catch_omp2
          }
        } _cimg_abort_catch_omp
      }
      cimg_abort_test;
      return res;