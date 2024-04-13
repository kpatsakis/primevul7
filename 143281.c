    template<typename t>
    inline t min(const t& a, const t& b, const t& c, const t& d) {
      return std::min(std::min(a,b),std::min(c,d));