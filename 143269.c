      }
      static bool is_nan(const half val) { // Custom version that works with '-ffast-math'
        if (sizeof(half)==2) {
          short u;
          std::memcpy(&u,&val,sizeof(short));
          return (bool)((u&0x7fff)>0x7c00);
        }
        return cimg::type<float>::is_nan((float)val);