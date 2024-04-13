  PosibErr<const KeyInfo *> Config::keyinfo(ParmStr key) const
  {
    typedef PosibErr<const KeyInfo *> Ret;
    {
      const KeyInfo * i;
      i = acommon::find(key, keyinfo_begin, keyinfo_end);
      if (i != keyinfo_end) return Ret(i);
      
      i = acommon::find(key, extra_begin, extra_end);
      if (i != extra_end) return Ret(i);
      
      const char * s = strncmp(key, "f-", 2) == 0 ? key + 2 : key.str();
      const char * h = strchr(s, '-');
      if (h == 0) goto err;

      String k(s, h - s);
      const ConfigModule * j = acommon::find(k,
					     filter_modules.pbegin(),
					     filter_modules.pend());
      
      if (j == filter_modules.pend() && load_filter_hook && committed_) {
        // FIXME: This isn't quite right
        PosibErrBase pe = load_filter_hook(const_cast<Config *>(this), k);
        pe.ignore_err();
        j = acommon::find(k,
                          filter_modules.pbegin(),
                          filter_modules.pend());
      }

      if (j == filter_modules.pend()) goto err;

      i = acommon::find(key, j->begin, j->end);
      if (i != j->end) return Ret(i);
      
      if (strncmp(key, "f-", 2) != 0) k = "f-";
      else                            k = "";
      k += key;
      i = acommon::find(k, j->begin, j->end);
      if (i != j->end) return Ret(i);
    }
  err:  
    return Ret().prim_err(unknown_key, key);
  }