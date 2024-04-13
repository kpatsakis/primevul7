    **/
    CImg<T>& normalize(const T& min_value, const T& max_value) {
      if (is_empty()) return *this;
      const T a = min_value<max_value?min_value:max_value, b = min_value<max_value?max_value:min_value;
      T m, M = max_min(m);
      const Tfloat fm = (Tfloat)m, fM = (Tfloat)M;
      if (m==M) return fill(min_value);
      if (m!=a || M!=b) cimg_rof(*this,ptrd,T) *ptrd = (T)((*ptrd - fm)/(fM - fm)*(b - a) + a);
      return *this;