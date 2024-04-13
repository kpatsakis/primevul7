    PossibleElementsEmul(const Config * d, bool ic, bool im)
      : include_extra(ic), include_modules(im), 
        module_changed(false), cd(d), i(d->keyinfo_begin), m(0) {}