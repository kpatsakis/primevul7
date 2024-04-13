    template<typename t>
    inline t min(const t& a, const t& b, const t& c) {
      return std::min(std::min(a,b),c);