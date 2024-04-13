  PosibErr<bool> Config::read_in_settings(const Config * other)
  {
    if (settings_read_in_) return false;

    bool was_committed = committed_;
    set_committed_state(false);

    if (other && other->settings_read_in_) {

      assert(empty());
      del(); // to clean up any notifiers and similar stuff
      copy(*other);

    } else {

      if (other) merge(*other);

      const char * env = getenv("ASPELL_CONF");
      if (env != 0) { 
        insert_point_ = &first_;
        RET_ON_ERR(read_in_string(env, _("ASPELL_CONF env var")));
      }
      
      {
      insert_point_ = &first_;
      PosibErrBase pe = read_in_file(retrieve("per-conf-path"));
      if (pe.has_err() && !pe.has_err(cant_read_file)) return pe;
      }
      
      {
        insert_point_ = &first_;
        PosibErrBase pe = read_in_file(retrieve("conf-path"));
        if (pe.has_err() && !pe.has_err(cant_read_file)) return pe;
      }

      if (was_committed)
        RET_ON_ERR(commit_all());

      settings_read_in_ = true;
    }

    return true;
  }