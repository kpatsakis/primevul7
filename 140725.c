    if (iTask.do_retry) {
        iTask.complete = 0;
        goto retry;
    }