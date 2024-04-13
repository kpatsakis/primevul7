
      static double mp_stov(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2);
        const ulongT siz = (ulongT)mp.opcode[3];
        longT ind = (longT)_mp_arg(4);
        const bool is_strict = (bool)_mp_arg(5);
        double val = cimg::type<double>::nan();
        if (ind<0 || ind>=(longT)siz) return val;
        if (!siz) return *ptrs>='0' && *ptrs<='9'?*ptrs - '0':val;

        CImg<charT> ss(siz + 1 - ind);
        ptrs+=1 + ind;
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;

        const char *s = ss._data;
        while (*s && *s<=32) ++s;
        const bool is_negative = *s=='-';
        if (is_negative || *s=='+') ++s;
        int err = 0;
        char sep;

        if (*s=='0' && (s[1]=='x' || s[1]=='X') && s[2]>32) { // Hexadecimal number
          unsigned int ival;
          err = cimg_sscanf(s + 2,"%x%c",&ival,&sep);
          if (err>0) val = (double)ival;
        } else if (*s>32) { // Decimal number
          err = cimg_sscanf(s,"%lf%c",&val,&sep);
#if cimg_OS==2
          // Check for +/-NaN and +/-inf as Microsoft's sscanf() version is not able
          // to read those particular values.
          if (!err && (*s=='i' || *s=='I' || *s=='n' || *s=='N')) {
            if (!cimg::strncasecmp(s,"inf",3)) { val = cimg::type<double>::inf(); err = 1 + (s[3]!=0); }
            else if (!cimg::strncasecmp(s,"nan",3)) { val = cimg::type<double>::nan(); err = 1 + (s[3]!=0); }
          }
#endif
        }
        if (err<=0 || (is_strict && err!=1)) return cimg::type<double>::nan();
        if (is_negative) val = -val;
        return val;