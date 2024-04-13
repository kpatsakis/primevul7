    PerIsolateThreadData(Isolate* isolate, ThreadId thread_id)
        : isolate_(isolate),
          thread_id_(thread_id),
          stack_limit_(0),
          thread_state_(NULL),
#if !defined(__arm__) && defined(V8_TARGET_ARCH_ARM) || \
    !defined(__mips__) && defined(V8_TARGET_ARCH_MIPS)
          simulator_(NULL),
#endif
          next_(NULL),
          prev_(NULL) { }