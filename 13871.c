static void reds_add_char_device(RedsState *reds, const red::shared_ptr<RedCharDevice> &dev)
{
    reds->char_devices.push_front(dev);
}