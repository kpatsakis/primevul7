  bool IsBelowFrame(JavaScriptFrame* frame) {
    return (c_entry_fp_ == 0) || (c_entry_fp_ > frame->sp());
  }