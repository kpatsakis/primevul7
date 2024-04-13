vg_virgl_poll(VuDev *dev, int condition, void *data)
{
    virgl_renderer_poll();
}