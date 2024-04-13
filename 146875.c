  ~SaveContext() {
    if (context_.is_null()) {
      Isolate* isolate = Isolate::Current();
      isolate->set_context(NULL);
      isolate->set_save_context(prev_);
    } else {
      Isolate* isolate = context_->GetIsolate();
      isolate->set_context(*context_);
      isolate->set_save_context(prev_);
    }
  }