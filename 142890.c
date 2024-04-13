    template<typename t1, typename t2>
    CImg<intT> _matchpatch(const CImg<T>& patch_image,
                           const unsigned int patch_width,
                           const unsigned int patch_height,
                           const unsigned int patch_depth,
                           const unsigned int nb_iterations,
                           const unsigned int nb_randoms,
                           const float occ_penalization,
                           const CImg<t1> &guide,
                           const bool is_matching_score,
                           CImg<t2> &matching_score) const {
      if (is_empty()) return CImg<intT>::const_empty();
      if (patch_image._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Instance image and specified patch image (%u,%u,%u,%u,%p) "
                                    "have different spectrums.",
                                    cimg_instance,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      if (patch_width>_width || patch_height>_height || patch_depth>_depth)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the instance image.",
                                    cimg_instance,patch_width,patch_height,patch_depth);
      if (patch_width>patch_image._width || patch_height>patch_image._height || patch_depth>patch_image._depth)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the patch image image (%u,%u,%u,%u,%p).",
                                    cimg_instance,patch_width,patch_height,patch_depth,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      const unsigned int
        _constraint = patch_image._depth>1?3:2,
        constraint = guide._spectrum>_constraint?_constraint:0;

      if (guide &&
          (guide._width!=_width || guide._height!=_height || guide._depth!=_depth || guide._spectrum<_constraint))
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified guide (%u,%u,%u,%u,%p) has invalid dimensions "
                                    "considering instance and patch image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);

      CImg<intT> a_map(_width,_height,_depth,patch_image._depth>1?3:2);
      CImg<ucharT> is_updated(_width,_height,_depth,1,3);
      CImg<floatT> score(_width,_height,_depth);
      CImg<uintT> occ, loop_order;
      ulongT rng = (cimg::_rand(),cimg::rng());
      if (occ_penalization!=0) {
        occ.assign(patch_image._width,patch_image._height,patch_image._depth,1,0);
        loop_order.assign(_width,_height,_depth,_depth>1?3:2);
        cimg_forXYZ(loop_order,x,y,z) {
          loop_order(x,y,z,0) = x;
          loop_order(x,y,z,1) = y;
          if (loop_order._spectrum>2) loop_order(x,y,z,2) = z;
        }
        cimg_forXYZ(loop_order,x,y,z) { // Randomize loop order in case of constraints on patch occurrence
          const unsigned int
            X = (unsigned int)cimg::round(cimg::rand(loop_order._width - 1.,&rng)),
            Y = (unsigned int)cimg::round(cimg::rand(loop_order._height - 1.,&rng)),
            Z = loop_order._depth>1?(unsigned int)cimg::round(cimg::rand(loop_order._depth  - 1.,&rng)):0U;
          cimg::swap(loop_order(x,y,z,0),loop_order(X,Y,Z,0));
          cimg::swap(loop_order(x,y,z,1),loop_order(X,Y,Z,1));
          if (loop_order._spectrum>2) cimg::swap(loop_order(x,y,z,2),loop_order(X,Y,Z,2));
        }
      }
      const int
        psizew = (int)patch_width,  psizew1 = psizew/2, psizew2 = psizew - psizew1 - 1,
        psizeh = (int)patch_height, psizeh1 = psizeh/2, psizeh2 = psizeh - psizeh1 - 1,
        psized = (int)patch_depth,  psized1 = psized/2, psized2 = psized - psized1 - 1;

      // Interleave image buffers to speed up patch comparison (cache-friendly).
      CImg<T> in_this = get_permute_axes("cxyz");
      in_this._width = _width*_spectrum;
      in_this._height = _height;
      in_this._depth = _depth;
      in_this._spectrum = 1;
      CImg<T> in_patch = patch_image.get_permute_axes("cxyz");
      in_patch._width = patch_image._width*patch_image._spectrum;
      in_patch._height = patch_image._height;
      in_patch._depth = patch_image._depth;
      in_patch._spectrum = 1;

      if (_depth>1 || patch_image._depth>1) { // 3D version

        // Initialize correspondence map.
        if (guide)
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_width,64))
            cimg_forXYZ(*this,x,y,z) { // User-defined initialization
            const int
              cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
              u = cimg::cut((int)guide(x,y,z,0),cx1,patch_image.width() - 1 - cx2),
              v = cimg::cut((int)guide(x,y,z,1),cy1,patch_image.height() - 1 - cy2),
              w = cimg::cut((int)guide(x,y,z,2),cz1,patch_image.depth() - 1 - cz2);
            a_map(x,y,z,0) = u;
            a_map(x,y,z,1) = v;
            a_map(x,y,z,2) = w;
            score(x,y,z) = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                       x - cx1,y - cy1,z - cz1,
                                       u - cx1,v - cy1,w - cz1,
                                       u,v,w,0,cimg::type<float>::inf());
          } else cimg_pragma_openmp(parallel cimg_openmp_if_size(_width,64)) {
            ulongT _rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            _rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for cimg_openmp_collapse(2))
              cimg_forXYZ(*this,x,y,z) { // Random initialization
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
                u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2,&_rng)),
                v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2,&_rng)),
                w = (int)cimg::round(cimg::rand(cz1,patch_image.depth() - 1 - cz2,&_rng));
              a_map(x,y,z,0) = u;
              a_map(x,y,z,1) = v;
              a_map(x,y,z,2) = w;
              score(x,y,z) = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                         x - cx1,y - cy1,z - cz1,
                                         u - cx1,v - cy1,w - cz1,
                                         u,v,w,0,cimg::type<float>::inf());
            }
            cimg::srand(_rng);
          }

        // Start iteration loop.
        cimg_abort_init;
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          cimg_abort_test;
          const bool is_odd = iter%2;
          const unsigned int cmask = is_odd?1:2, nmask = 3 - cmask;
          if (iter) occ.fill(0);

          cimg_pragma_openmp(parallel cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                     iter<nb_iterations-2)) {
            ulongT _rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            _rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for cimg_openmp_collapse(2))
              cimg_forXYZ(*this,X,Y,Z) {
              const int
                _x = is_odd?width() - 1 - X:X,
                _y = is_odd?height() - 1 - Y:Y,
                _z = is_odd?depth() - 1 - Z:Z;
              int x, y, z;
              if (occ_penalization) {
                x = loop_order(_x,_y,_z,0);
                y = loop_order(_x,_y,_z,1);
                if (loop_order._spectrum>2) z = loop_order(_x,_y,_z,2); else z = _z;
              } else { x = _x; y = _y; z = _z; }

              if (score(x,y,z)<=1e-5 || (constraint && guide(x,y,z,constraint)!=0)) continue;
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
                xp = x - cx1,
                yp = y - cy1,
                zp = z - cz1;

              int best_u = a_map(x,y,z,0), best_v = a_map(x,y,z,1), best_w = a_map(x,y,z,2), u, v, w;
              const float best_score0 = score(x,y,z);
              float best_score = best_score0, s;

              // Propagation.
              if (x>0 && (is_updated(x - 1,y,z)&cmask)) { // Compare with left neighbor
                u = a_map(x - 1,y,z,0);
                v = a_map(x - 1,y,z,1);
                w = a_map(x - 1,y,z,2);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u + 1 - cx1,v - cy1,w - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u + 1; best_v = v; best_w = w; best_score = s; }
                }
              }
              if (y>0 && (is_updated(x,y - 1,z)&cmask)) { // Compare with up neighbor
                u = a_map(x,y - 1,z,0);
                v = a_map(x,y - 1,z,1);
                w = a_map(x,y - 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v + 1 - cy1,w - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v + 1; best_w = w; best_score = s; }
                }
              }
              if (z>0 && (is_updated(x,y,z - 1)&cmask)) { // Compare with backward neighbor
                u = a_map(x,y,z - 1,0);
                v = a_map(x,y,z - 1,1);
                w = a_map(x,y,z - 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 - 1 && w<patch_image.depth() - 1 - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w + 1 - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w + 1; best_score = s; }
                }
              }
              if (x<width() - 1 && (is_updated(x + 1,y,z)&cmask)) { // Compare with right neighbor
                u = a_map(x + 1,y,z,0);
                v = a_map(x + 1,y,z,1);
                w = a_map(x + 1,y,z,2);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - 1 - cx1,v - cy1,w - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u - 1; best_v = v; best_w = w; best_score = s; }
                }
              }
              if (y<height() - 1 && (is_updated(x,y + 1,z)&cmask)) { // Compare with bottom neighbor
                u = a_map(x,y + 1,z,0);
                v = a_map(x,y + 1,z,1);
                w = a_map(x,y + 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - 1 - cy1,w - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v - 1; best_w = w; best_score = s; }
                }
              }
              if (z<depth() - 1 && (is_updated(x,y,z + 1)&cmask)) { // Compare with forward neighbor
                u = a_map(x,y,z + 1,0);
                v = a_map(x,y,z + 1,1);
                w = a_map(x,y,z + 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 + 1 && w<patch_image.depth() + 1 - cz2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w - 1 - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w - 1; best_score = s; }
                }
              }

              // Randomization.
              float
                dw = (float)patch_image.width(),
                dh = (float)patch_image.height(),
                dd = (float)patch_image.depth();
              for (unsigned int i = 0; i<nb_randoms; ++i) {
                u = (int)cimg::round(cimg::rand(std::max((float)cx1,best_u - dw),
                                                std::min(patch_image.width() - 1.f - cx2,best_u + dw),&_rng));
                v = (int)cimg::round(cimg::rand(std::max((float)cy1,best_v - dh),
                                                std::min(patch_image.height() - 1.f - cy2,best_v + dh),&_rng));
                w = (int)cimg::round(cimg::rand(std::max((float)cz1,best_w - dd),
                                                std::min(patch_image.depth() - 1.f - cz2,best_w + dd),&_rng));
                if (u!=best_u || v!=best_v || w!=best_w) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w - cz1,
                                  u,v,w,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w; best_score = s; }
                  dw = std::max(5.f,dw*0.5f); dh = std::max(5.f,dh*0.5f); dd = std::max(5.f,dd*0.5f);
                }
              }

              if (best_score<best_score0) {
                a_map(x,y,z,0) = best_u;
                a_map(x,y,z,1) = best_v;
                a_map(x,y,z,2) = best_w;
                score(x,y,z) = best_score;
                is_updated(x,y,z) = 3;
              } else is_updated(x,y,z)&=~nmask;
              if (occ_penalization!=0) cimg_pragma_openmp(atomic) ++occ(best_u,best_v,best_w);
            }
            cimg::srand(_rng);
          }
        }

      } else { // 2D version

        // Initialize correspondence map.
        if (guide)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(_width,64))
            cimg_forXY(*this,x,y) { // User-defined initialization
            const int
              cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              u = cimg::cut((int)guide(x,y,0),cx1,patch_image.width() - 1 - cx2),
              v = cimg::cut((int)guide(x,y,1),cy1,patch_image.height() - 1 - cy2);
            a_map(x,y,0) = u;
            a_map(x,y,1) = v;
            score(x,y) = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                     x - cx1,y - cy1,u - cx1,v - cy1,
                                     u,v,0,cimg::type<float>::inf());
          } else cimg_pragma_openmp(parallel cimg_openmp_if_size(_width,64)) {
            ulongT _rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            _rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for)
              cimg_forXY(*this,x,y) { // Random initialization
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2,&_rng)),
                v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2,&_rng));
              a_map(x,y,0) = u;
              a_map(x,y,1) = v;
              score(x,y) = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                       x - cx1,y - cy1,u - cx1,v - cy1,
                                       u,v,0,cimg::type<float>::inf());
            }
            cimg::srand(_rng);
          }

        // Start iteration loop.
        cimg_abort_init;
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          cimg_abort_test;
          const bool is_odd = iter%2;
          const unsigned int cmask = is_odd?1:2, nmask = 3 - cmask;
          if (iter) occ.fill(0);

          cimg_pragma_openmp(parallel cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                     iter<nb_iterations-2)) {
            ulongT _rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            _rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for)
              cimg_forXY(*this,X,Y) {
              const int
                _x = is_odd?width() - 1 - X:X,
                _y = is_odd?height() - 1 - Y:Y;
              int x, y;
              if (occ_penalization) {
                x = loop_order(_x,_y,0);
                y = loop_order(_x,_y,1);
              } else { x = _x; y = _y; }

              if (score(x,y)<=1e-5 || (constraint && guide(x,y,constraint)!=0)) continue;
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                xp = x - cx1,
                yp = y - cy1;

              int best_u = a_map(x,y,0), best_v = a_map(x,y,1), u, v;
              const float best_score0 = score(x,y);
              float best_score = best_score0, s;

              // Propagation.
              if (x>0 && (is_updated(x - 1,y)&cmask)) { // Compare with left neighbor
                u = a_map(x - 1,y,0);
                v = a_map(x - 1,y,1);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                  xp,yp,u + 1 - cx1,v - cy1,
                                  u,v,occ_penalization,best_score);
                  if (s<best_score) { best_u = u + 1; best_v = v; best_score = s; }
                }
              }
              if (y>0 && (is_updated(x,y - 1)&cmask)) { // Compare with up neighbor
                u = a_map(x,y - 1,0);
                v = a_map(x,y - 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v + 1 - cy1,
                                  u,v,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v + 1; best_score = s; }
                }
              }
              if (x<width() - 1 && (is_updated(x + 1,y)&cmask)) { // Compare with right neighbor
                u = a_map(x + 1,y,0);
                v = a_map(x + 1,y,1);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                  xp,yp,u - 1 - cx1,v - cy1,
                                  u,v,occ_penalization,best_score);
                  if (s<best_score) { best_u = u - 1; best_v = v; best_score = s; }
                }
              }
              if (y<height() - 1 && (is_updated(x,y + 1)&cmask)) { // Compare with bottom neighbor
                u = a_map(x,y + 1,0);
                v = a_map(x,y + 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v - 1 - cy1,
                                  u,v,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v - 1; best_score = s; }
                }
              }

              // Randomization.
              float
                dw = (float)patch_image.width(),
                dh = (float)patch_image.height();
              for (unsigned int i = 0; i<nb_randoms; ++i) {
                u = (int)cimg::round(cimg::rand(std::max((float)cx1,best_u - dw),
                                                std::min(patch_image.width() - 1.f - cx2,best_u + dw),&_rng));
                v = (int)cimg::round(cimg::rand(std::max((float)cy1,best_v - dh),
                                                std::min(patch_image.height() - 1.f - cy2,best_v + dh),&_rng));
                if (u!=best_u || v!=best_v) {
                  s = _matchpatch(in_this,in_patch,occ,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v - cy1,
                                  u,v,occ_penalization,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_score = s; }
                  dw = std::max(5.f,dw*0.5f); dh = std::max(5.f,dh*0.5f);
                }
              }

              if (best_score<best_score0) {
                a_map(x,y,0) = best_u;
                a_map(x,y,1) = best_v;
                score(x,y) = best_score;
                is_updated(x,y) = 3;
              } else is_updated(x,y)&=~nmask;
              if (occ_penalization!=0) cimg_pragma_openmp(atomic) ++occ(best_u,best_v);
            }
          }
          cimg::srand(rng);
        }
      }
      if (is_matching_score) score.move_to(matching_score);
      return a_map;