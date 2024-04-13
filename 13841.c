MainDispatcher* reds_get_main_dispatcher(RedsState *reds)
{
    return reds->main_dispatcher.get();
}