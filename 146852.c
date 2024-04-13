  Handle<JSBuiltinsObject> js_builtins_object() {
    return Handle<JSBuiltinsObject>(thread_local_top_.context_->builtins());
  }