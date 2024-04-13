    template<typename T> struct type {
      static const char* string() {
        static const char* s[] = { "unknown",   "unknown8",   "unknown16",  "unknown24",
                                   "unknown32", "unknown40",  "unknown48",  "unknown56",
                                   "unknown64", "unknown72",  "unknown80",  "unknown88",
                                   "unknown96", "unknown104", "unknown112", "unknown120",
                                   "unknown128" };
        return s[(sizeof(T)<17)?sizeof(T):0];