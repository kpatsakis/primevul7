    **/
    inline bool is_file(const char *const path) {
      if (!path || !*path) return false;
      std::FILE *const file = cimg::std_fopen(path,"rb");
      if (!file) return false;
      cimg::fclose(file);
      return !is_directory(path);