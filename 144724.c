    //! Return \c true if input character is blank (space, tab, or non-printable character).
    inline bool is_blank(const char c) {
      return c>=0 && c<=' ';