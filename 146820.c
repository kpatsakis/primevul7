inline void Context::mark_out_of_memory() {
  native_context()->set_out_of_memory(HEAP->true_value());
}