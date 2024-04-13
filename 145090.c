  PosibErr<void> Config::commit_all(Vector<int> * phs, const char * codeset)
  {
    committed_ = true;
    others_ = first_;
    first_ = 0;
    insert_point_ = &first_;
    Conv to_utf8;
    if (codeset)
      RET_ON_ERR(to_utf8.setup(*this, codeset, "utf-8", NormTo));
    while (others_) {
      *insert_point_ = others_;
      others_ = others_->next;
      (*insert_point_)->next = 0;
      RET_ON_ERR_SET(commit(*insert_point_, codeset ? &to_utf8 : 0), int, place_holder);
      if (phs && place_holder != -1 && (phs->empty() || phs->back() != place_holder))
        phs->push_back(place_holder);
      insert_point_ = &((*insert_point_)->next);
    }
    return no_err;
  }