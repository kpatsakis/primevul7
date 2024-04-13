  void set_context(Context* context) {
    ASSERT(context == NULL || context->IsContext());
    thread_local_top_.context_ = context;
  }