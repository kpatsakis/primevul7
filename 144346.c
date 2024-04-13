    template<typename t>
    inline t max(const t& a, const t& b, const t& c, const t& d) {
      return std::max(std::max(a,b),std::max(c,d));