      }
      static bool is_nan(const float val) { // Custom version that works with '-ffast-math'
        if (sizeof(float)==4) {
          unsigned int u;
          std::memcpy(&u,&val,sizeof(float));
          return (u&0x7fffffff)>0x7f800000;
        }
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif