static void megasas_reset_frames(MegasasState *s)
{
    int i;
    MegasasCmd *cmd;

    for (i = 0; i < s->fw_cmds; i++) {
        cmd = &s->frames[i];
        if (cmd->pa) {
            megasas_unmap_frame(s, cmd);
        }
    }
    bitmap_zero(s->frame_map, MEGASAS_MAX_FRAMES);
}