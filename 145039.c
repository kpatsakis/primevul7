  void Config::write_to_stream(OStream & out, 
			       bool include_extra) 
  {
    KeyInfoEnumeration * els = possible_elements(include_extra);
    const KeyInfo * i;
    String buf;
    String obuf;
    String def;
    bool have_value;

    while ((i = els->next()) != 0) {
      if (i->desc == 0) continue;

      if (els->active_filter_module_changed()) {
        out.printf(_("\n"
                     "#######################################################################\n"
                     "#\n"
                     "# Filter: %s\n"
                     "#   %s\n"
                     "#\n"
                     "# configured as follows:\n"
                     "\n"),
                   els->active_filter_module_name(),
                   _(els->active_filter_module_desc()));
      }

      obuf.clear();
      have_value = false;

      obuf.printf("# %s (%s)\n#   %s\n",
                  i->name, _(keyinfo_type_name[i->type]), _(i->desc));
      if (i->def != 0) {
	if (i->type != KeyInfoList) {
          buf.resize(strlen(i->def) * 2 + 1);
          escape(buf.data(), i->def);
          obuf.printf("# default: %s", buf.data());
          def = get_default(i);
          if (def != i->def) {
            buf.resize(def.size() * 2 + 1);
            escape(buf.data(), def.str());
            obuf.printf(" = %s", buf.data());
          }
          obuf << '\n';
          const Entry * entry = lookup(i->name);
	  if (entry) {
            have_value = true;
            buf.resize(entry->value.size() * 2 + 1);
            escape(buf.data(), entry->value.str());
	    obuf.printf("%s %s\n", i->name, buf.data());
          }
	} else {
          unsigned s = obuf.size();
          ListDump ld(obuf, i->name);
          lookup_list(i, ld, false);
          have_value = s != obuf.size();
	}
      }
      obuf << '\n';
      if (!(i->flags & KEYINFO_HIDDEN) || have_value)
        out.write(obuf);
    }
    delete els;
  }