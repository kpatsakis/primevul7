    template<typename T>
    inline int date(T *attr, const unsigned int nb_attr) {
      int res = -1;
      cimg::mutex(6);
#if cimg_OS==2
      SYSTEMTIME st;
      GetLocalTime(&st);
      for (unsigned int i = 0; i<nb_attr; ++i) {
        res = (int)(attr[i]==0?st.wYear:attr[i]==1?st.wMonth:attr[i]==2?st.wDay:
                    attr[i]==3?st.wDayOfWeek:attr[i]==4?st.wHour:attr[i]==5?st.wMinute:
                    attr[i]==6?st.wSecond:-1);
        attr[i] = (T)res;
      }
#else
      time_t _st;
      std::time(&_st);
      struct tm *st = std::localtime(&_st);
      for (unsigned int i = 0; i<nb_attr; ++i) {
        res = (int)(attr[i]==0?st->tm_year + 1900:attr[i]==1?st->tm_mon + 1:attr[i]==2?st->tm_mday:
                    attr[i]==3?st->tm_wday:attr[i]==4?st->tm_hour:attr[i]==5?st->tm_min:
                    attr[i]==6?st->tm_sec:-1);
        attr[i] = (T)res;
      }
#endif
      cimg::mutex(6,0);
      return res;