snmpAgentResponse(struct snmp_pdu *PDU) {

    struct snmp_pdu *Answer = NULL;

    debugs(49, 5, "snmpAgentResponse: Called.");

    if ((Answer = snmp_pdu_create(SNMP_PDU_RESPONSE))) {
        Answer->reqid = PDU->reqid;
        Answer->errindex = 0;

        if (PDU->command == SNMP_PDU_GET || PDU->command == SNMP_PDU_GETNEXT) {
            /* Indirect way */
            int get_next = (PDU->command == SNMP_PDU_GETNEXT);
            variable_list *VarPtr_;
            variable_list **RespVars = &(Answer->variables);
            oid_ParseFn *ParseFn;
            int index = 0;
            /* Loop through all variables */

            for (VarPtr_ = PDU->variables; VarPtr_; VarPtr_ = VarPtr_->next_variable) {
                variable_list *VarPtr = VarPtr_;
                variable_list *VarNew = NULL;
                oid *NextOidName = NULL;
                snint NextOidNameLen = 0;

                ++index;

                if (get_next)
                    ParseFn = snmpTreeNext(VarPtr->name, VarPtr->name_length, &NextOidName, &NextOidNameLen);
                else
                    ParseFn = snmpTreeGet(VarPtr->name, VarPtr->name_length);

                if (ParseFn == NULL) {
                    Answer->errstat = SNMP_ERR_NOSUCHNAME;
                    debugs(49, 5, "snmpAgentResponse: No such oid. ");
                } else {
                    if (get_next) {
                        VarPtr = snmp_var_new(NextOidName, NextOidNameLen);
                        xfree(NextOidName);
                    }

                    int * errstatTmp =  &(Answer->errstat);

                    VarNew = (*ParseFn) (VarPtr, (snint *) errstatTmp);

                    if (get_next)
                        snmp_var_free(VarPtr);
                }

                if ((Answer->errstat != SNMP_ERR_NOERROR) || (VarNew == NULL)) {
                    Answer->errindex = index;
                    debugs(49, 5, "snmpAgentResponse: error.");

                    if (VarNew)
                        snmp_var_free(VarNew);

                    while ((VarPtr = Answer->variables) != NULL) {
                        Answer->variables = VarPtr->next_variable;
                        snmp_var_free(VarPtr);
                    }

                    /* Steal the original PDU list of variables for the error response */
                    Answer->variables = PDU->variables;

                    PDU->variables = NULL;

                    return (Answer);
                }

                /* No error.  Insert this var at the end, and move on to the next.
                 */
                *RespVars = VarNew;

                RespVars = &(VarNew->next_variable);
            }
        }
    }

    return (Answer);
}