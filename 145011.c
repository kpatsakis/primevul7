  PosibErr<void> Config::set(Entry * entry0, bool do_unescape)
  {
    StackPtr<Entry> entry(entry0);

    if (entry->action == NoOp)
      entry->key = base_name(entry->key.str(), &entry->action);

    if (num_parms(entry->action) == 0 && !entry->value.empty()) 
    {
      if (entry->place_holder == -1) {
        switch (entry->action) {
        case Reset:
          return make_err(no_value_reset, entry->key);
        case Enable:
          return make_err(no_value_enable, entry->key);
        case Disable:
          return make_err(no_value_disable, entry->key);
        case ListClear:
          return make_err(no_value_clear, entry->key);
        default:
          abort(); // this shouldn't happen
        }
      } else {
        entry->place_holder = -1;
      }
    }

    if (entry->action != ListSet) {

      switch (entry->action) {
      case Enable:
        entry->value = "true";
        entry->action = Set;
        break;
      case Disable:
        entry->value = "false";
        entry->action = Set;
        break;
      default:
        ;
      }
      if (do_unescape) unescape(entry->value.mstr());

      entry->next = *insert_point_;
      *insert_point_ = entry;
      insert_point_ = &entry->next;
      entry.release();
      if (committed_) RET_ON_ERR(commit(entry0)); // entry0 == entry
      
    } else { // action == ListSet

      Entry * ent = new Entry;
      ent->key = entry->key;
      ent->action = ListClear;
      set(ent);

      ListAddHelper helper;
      helper.config = this;
      helper.orig_entry = entry;

      separate_list(entry->value.str(), helper, do_unescape);
    }
    return no_err;
  }