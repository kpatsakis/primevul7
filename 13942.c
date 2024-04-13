static void reds_remove_char_device(RedsState *reds, RedCharDevice *dev)
{
    g_return_if_fail(reds != NULL);
    auto &devs(reds->char_devices);
    g_warn_if_fail(std::find(devs.begin(), devs.end(),
                             red::shared_ptr<RedCharDevice>(dev)) != devs.end());

    devs.remove(red::shared_ptr<RedCharDevice>(dev));
}