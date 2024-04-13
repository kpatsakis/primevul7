static LEX_CSTRING event_to_str(unsigned int event_class,
                                unsigned long event_subclass)
{
  int count;
  for (count= 0; event_subclass; count++, event_subclass >>= 1);

  return event_names[event_class][count - 1];
}