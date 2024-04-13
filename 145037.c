  PosibErr<void> Config::set_committed_state(bool val) {
    if (val && !committed_) {
      RET_ON_ERR(commit_all());
    } else if (!val && committed_) {
      assert(empty());
      committed_ = false;
    }
    return no_err;
  }