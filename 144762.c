    template<typename t>
    const CImg<T>& SVD(CImg<t>& U, CImg<t>& S, CImg<t>& V, const bool sorting=true,
                       const unsigned int max_iteration=40, const float lambda=0) const {
      typedef _cimg_Ttfloat Ttfloat;

      if (is_empty()) { U.assign(); S.assign(); V.assign(); }
      else {
        U = *this;
	if (lambda!=0) {
	  const unsigned int delta = std::min(U._width,U._height);
	  for (unsigned int i = 0; i<delta; ++i) U(i,i) = (t)(U(i,i) + lambda);
	}
        if (S.size()<_width) S.assign(1,_width);
        if (V._width<_width || V._height<_height) V.assign(_width,_width);
        CImg<t> rv1(_width);
        Ttfloat anorm = 0, c, f, g = 0, h, s, scale = 0;
        int l = 0, nm = 0;

        cimg_forX(U,i) {
          l = i + 1; rv1[i] = scale*g; g = s = scale = 0;
          if (i<height()) {
            for (int k = i; k<height(); ++k) scale+=cimg::abs(U(i,k));
            if (scale) {
              for (int k = i; k<height(); ++k) { U(i,k)/=scale; s+=U(i,k)*U(i,k); }
              f = U(i,i); g = (Ttfloat)((f>=0?-1:1)*std::sqrt(s)); h=f*g-s; U(i,i) = f-g;
              for (int j = l; j<width(); ++j) {
                s = 0;
                for (int k=i; k<height(); ++k) s+=U(i,k)*U(j,k);
                f = s/h;
                for (int k = i; k<height(); ++k) U(j,k)+=f*U(i,k);
              }
              for (int k = i; k<height(); ++k) U(i,k)*=scale;
            }
          }
          S[i]=scale*g;

          g = s = scale = 0;
          if (i<height() && i!=width() - 1) {
            for (int k = l; k<width(); ++k) scale+=cimg::abs(U(k,i));
            if (scale) {
              for (int k = l; k<width(); ++k) { U(k,i)/= scale; s+=U(k,i)*U(k,i); }
              f = U(l,i); g = (Ttfloat)((f>=0?-1:1)*std::sqrt(s)); h = f*g-s; U(l,i) = f-g;
              for (int k = l; k<width(); ++k) rv1[k]=U(k,i)/h;
              for (int j = l; j<height(); ++j) {
                s = 0;
                for (int k = l; k<width(); ++k) s+=U(k,j)*U(k,i);
                for (int k = l; k<width(); ++k) U(k,j)+=s*rv1[k];
              }
              for (int k = l; k<width(); ++k) U(k,i)*=scale;
            }
          }
          anorm = (Ttfloat)std::max((float)anorm,(float)(cimg::abs(S[i]) + cimg::abs(rv1[i])));
        }

        for (int i = width() - 1; i>=0; --i) {
          if (i<width()-1) {
            if (g) {
              for (int j = l; j<width(); ++j) V(i,j) =(U(j,i)/U(l,i))/g;
              for (int j = l; j<width(); ++j) {
                s = 0;
                for (int k = l; k<width(); ++k) s+=U(k,i)*V(j,k);
                for (int k = l; k<width(); ++k) V(j,k)+=s*V(i,k);
              }
            }
            for (int j = l; j<width(); ++j) V(j,i) = V(i,j) = (t)0.;
          }
          V(i,i) = (t)1.; g = rv1[i]; l = i;
        }

        for (int i = std::min(width(),height()) - 1; i>=0; --i) {
          l = i + 1; g = S[i];
          for (int j = l; j<width(); ++j) U(j,i) = 0;
          if (g) {
            g = 1/g;
            for (int j = l; j<width(); ++j) {
              s = 0; for (int k = l; k<height(); ++k) s+=U(i,k)*U(j,k);
              f = (s/U(i,i))*g;
              for (int k = i; k<height(); ++k) U(j,k)+=f*U(i,k);
            }
            for (int j = i; j<height(); ++j) U(i,j)*= g;
          } else for (int j = i; j<height(); ++j) U(i,j) = 0;
          ++U(i,i);
        }

        for (int k = width() - 1; k>=0; --k) {
          for (unsigned int its = 0; its<max_iteration; ++its) {
            bool flag = true;
            for (l = k; l>=1; --l) {
              nm = l - 1;
              if ((cimg::abs(rv1[l]) + anorm)==anorm) { flag = false; break; }
              if ((cimg::abs(S[nm]) + anorm)==anorm) break;
            }
            if (flag) {
              c = 0; s = 1;
              for (int i = l; i<=k; ++i) {
                f = s*rv1[i]; rv1[i] = c*rv1[i];
                if ((cimg::abs(f) + anorm)==anorm) break;
                g = S[i]; h = cimg::_hypot(f,g); S[i] = h; h = 1/h; c = g*h; s = -f*h;
                cimg_forY(U,j) { const t y = U(nm,j), z = U(i,j); U(nm,j) = y*c + z*s; U(i,j) = z*c - y*s; }
              }
            }

            const t z = S[k];
            if (l==k) { if (z<0) { S[k] = -z; cimg_forX(U,j) V(k,j) = -V(k,j); } break; }
            nm = k - 1;
            t x = S[l], y = S[nm];
            g = rv1[nm]; h = rv1[k];
            f = ((y - z)*(y + z)+(g - h)*(g + h))/std::max((Ttfloat)1e-25,(Ttfloat)2*h*y);
            g = cimg::_hypot(f,(Ttfloat)1);
            f = ((x - z)*(x + z)+h*((y/(f + (f>=0?g:-g))) - h))/std::max((Ttfloat)1e-25,(Ttfloat)x);
            c = s = 1;
            for (int j = l; j<=nm; ++j) {
              const int i = j + 1;
              g = rv1[i]; h = s*g; g = c*g;
              t y1 = S[i];
              t z1 = cimg::_hypot(f,h);
              rv1[j] = z1; c = f/std::max((Ttfloat)1e-25,(Ttfloat)z1); s = h/std::max((Ttfloat)1e-25,(Ttfloat)z1);
              f = x*c + g*s; g = g*c - x*s; h = y1*s; y1*=c;
              cimg_forX(U,jj) { const t x2 = V(j,jj), z2 = V(i,jj); V(j,jj) = x2*c + z2*s; V(i,jj) = z2*c - x2*s; }
              z1 = cimg::_hypot(f,h); S[j] = z1;
              if (z1) { z1 = 1/std::max((Ttfloat)1e-25,(Ttfloat)z1); c = f*z1; s = h*z1; }
              f = c*g + s*y1; x = c*y1 - s*g;
              cimg_forY(U,jj) { const t y2 = U(j,jj), z2 = U(i,jj); U(j,jj) = y2*c + z2*s; U(i,jj) = z2*c - y2*s; }
            }
            rv1[l] = 0; rv1[k] = f; S[k] = x;
          }
        }

        if (sorting) {
          CImg<intT> permutations;
          CImg<t> tmp(_width);
          S.sort(permutations,false);
          cimg_forY(U,k) {
            cimg_forY(permutations,y) tmp(y) = U(permutations(y),k);
            std::memcpy(U.data(0,k),tmp._data,sizeof(t)*_width);
          }
          cimg_forY(V,k) {
            cimg_forY(permutations,y) tmp(y) = V(permutations(y),k);
            std::memcpy(V.data(0,k),tmp._data,sizeof(t)*_width);
          }
        }
      }
      return *this;