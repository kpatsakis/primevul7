    template<typename T>
    inline size_t fread(T *const ptr, const size_t nmemb, std::FILE *stream) {
      if (!ptr || !stream)
        throw CImgArgumentException("cimg::fread(): Invalid reading request of %u %s%s from file %p to buffer %p.",
                                    nmemb,cimg::type<T>::string(),nmemb>1?"s":"",stream,ptr);
      if (!nmemb) return 0;
      const size_t wlimitT = 63*1024*1024, wlimit = wlimitT/sizeof(T);
      size_t to_read = nmemb, al_read = 0, l_to_read = 0, l_al_read = 0;
      do {
        l_to_read = (to_read*sizeof(T))<wlimitT?to_read:wlimit;
        l_al_read = std::fread((void*)(ptr + al_read),sizeof(T),l_to_read,stream);
        al_read+=l_al_read;
        to_read-=l_al_read;
      } while (l_to_read==l_al_read && to_read>0);
      if (to_read>0)
        warn("cimg::fread(): Only %lu/%lu elements could be read from file.",
             (unsigned long)al_read,(unsigned long)nmemb);
      return al_read;