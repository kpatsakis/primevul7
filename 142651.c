netsnmp_state_machine_run( netsnmp_state_machine_input *input)
{
    netsnmp_state_machine_step *current, *last;

    netsnmp_require_ptr_LRV( input, SNMPERR_GENERR );
    netsnmp_require_ptr_LRV( input->steps, SNMPERR_GENERR );
    last = current = input->steps;

    DEBUGMSGT(("state_machine:run", "starting step: %s\n", current->name));

    while (current) {

        /*
         * log step and check for required data
         */
        DEBUGMSGT(("state_machine:run", "at step: %s\n", current->name));
        if (NULL == current->run) {
            DEBUGMSGT(("state_machine:run", "no run step\n"));
            current->result = last->result;
            break;
        }

        /*
         * run step
         */
        DEBUGMSGT(("state_machine:run", "running step: %s\n", current->name));
        current->result = (*current->run)( input, current );
        ++input->steps_so_far;
        
        /*
         * log result and move to next step
         */
        input->last_run = current;
        DEBUGMSGT(("state_machine:run:result", "step %s returned %d\n",
                   current->name, current->result));
        if (SNMPERR_SUCCESS == current->result)
            current = current->on_success;
        else if (SNMPERR_ABORT == current->result) {
            DEBUGMSGT(("state_machine:run:result", "ABORT from %s\n",
                       current->name));
            break;
        }
        else
            current = current->on_error;
    }

    /*
     * run cleanup
     */
    if ((input->cleanup) && (input->cleanup->run))
        (*input->cleanup->run)( input, input->last_run );

    return input->last_run->result;
}