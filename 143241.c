      // Return true if specified argument can be a part of an allowed  variable name.
      bool is_varchar(const char c) const {
        return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';