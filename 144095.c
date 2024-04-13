      }
      static bool is_nan(const double val) { // Custom version that works with '-ffast-math'
        if (sizeof(double)==8) {
          cimg_uint64 u;
          std::memcpy(&u,&val,sizeof(double));
          return ((unsigned int)(u>>32)&0x7fffffff) + ((unsigned int)u!=0)>0x7ff00000;
        }
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif