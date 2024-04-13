void stat_remove_counter(SpiceServer *reds, RedStatCounter *counter)
{
    if (counter->counter) {
        stat_file_remove_counter(reds->stat_file, counter->counter);
        counter->counter = NULL;
    }
}