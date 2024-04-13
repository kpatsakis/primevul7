  PosibErr<int> Config::commit(Entry * entry, Conv * conv) 
  {
    PosibErr<const KeyInfo *> pe = keyinfo(entry->key);
    {
      if (pe.has_err()) goto error;
      
      const KeyInfo * ki = pe;

      entry->key = ki->name;
      
      // FIXME: This is the correct thing to do but it causes problems
      //        with changing a filter mode in "pipe" mode and probably
      //        elsewhere.
      //if (attached_ && !(ki->flags & KEYINFO_MAY_CHANGE)) {
      //  pe = make_err(cant_change_value, entry->key);
      //  goto error;
      //}

      int place_holder = entry->place_holder;
      
      if (conv && ki->flags & KEYINFO_UTF8)
        entry->value = (*conv)(entry->value);

      if (ki->type != KeyInfoList && list_action(entry->action)) {
        pe = make_err(key_not_list, entry->key);
        goto error;
      }
      
      assert(ki->def != 0); // if null this key should never have values
      // directly added to it
      String value(entry->action == Reset ? get_default(ki) : entry->value);
      
      switch (ki->type) {
        
      case KeyInfoBool: {

        bool val;
      
        if  (value.empty() || entry->place_holder != -1) {
          // if entry->place_holder != -1 than IGNORE the value no
          // matter what it is
          entry->value = "true";
          val = true;
          place_holder = -1;
        } else if (value == "true") {
          val = true;
        } else if (value == "false") {
          val = false;
        } else {
          pe = make_err(bad_value, entry->key, value,
                        /* TRANSLATORS: "true" and "false" are literal
                         * values and should not be translated.*/
                        _("either \"true\" or \"false\""));
          goto error;
        }

        NOTIFY_ALL(item_updated(ki, val));
        break;
        
      } case KeyInfoString:
        
        NOTIFY_ALL(item_updated(ki, value));
        break;
        
      case KeyInfoInt: 
      {
        int num;
        
        if (sscanf(value.str(), "%i", &num) == 1 && num >= 0) {
          NOTIFY_ALL(item_updated(ki, num));
        } else {
          pe = make_err(bad_value, entry->key, value, _("a positive integer"));
          goto error;
        }
        
        break;
      }
      case KeyInfoList:
        
        NOTIFY_ALL(list_updated(ki));
        break;
        
      }
      return place_holder;
    }
  error:
    entry->action = NoOp;
    if (!entry->file.empty())
      return pe.with_file(entry->file, entry->line_num);
    else
      return (PosibErrBase &)pe;
  }