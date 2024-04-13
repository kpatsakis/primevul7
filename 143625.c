    template<typename T1, typename T2, typename T3, typename T4>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4) {
      cimg::swap(a1,b1,a2,b2,a3,b3); cimg::swap(a4,b4);