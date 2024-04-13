    const KeyInfo * next() {
      if (i == cd->keyinfo_end) {
	if (include_extra)
	  i = cd->extra_begin;
	else
	  i = cd->extra_end;
      }
      
      module_changed = false;
      if (i == cd->extra_end) {
	m = cd->filter_modules.pbegin();
	if (!include_modules || m == cd->filter_modules.pend()) return 0;
	else {
          i = m->begin;
          module_changed = true;
        }
      }

      if (m == 0){
	return i++;
      }

      if (m == cd->filter_modules.pend()){
	return 0;
      }

      while (i == m->end) {
	++m;
	if (m == cd->filter_modules.pend()) return 0;
	else {
          i = m->begin;
          module_changed = true;
        }
      }

      return i++;
    }