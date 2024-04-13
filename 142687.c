usm_check_and_update_timeliness(u_char * secEngineID,
                                size_t secEngineIDLen,
                                u_int boots_uint,
                                u_int time_uint, int *error)
{
    u_char          myID[USM_MAX_ID_LENGTH];
    u_long          myIDLength =
        snmpv3_get_engineID(myID, USM_MAX_ID_LENGTH);
    u_int           myBoots;
    u_int           myTime;



    if ((myIDLength > USM_MAX_ID_LENGTH) || (myIDLength == 0)) {
        /*
         * We're probably already screwed...buffer overwrite.  XXX? 
         */
        DEBUGMSGTL(("usm", "Buffer overflow.\n"));
        *error = SNMPERR_USM_GENERICERROR;
        return -1;
    }

    myBoots = snmpv3_local_snmpEngineBoots();
    myTime = snmpv3_local_snmpEngineTime();


    /*
     * IF the time involved is local
     *     Make sure  message is inside the time window 
     * ELSE 
     *      IF boots is higher or boots is the same and time is higher
     *              remember this new data
     *      ELSE
     *              IF !(boots same and time within USM_TIME_WINDOW secs)
     *                      Message is too old 
     *              ELSE    
     *                      Message is ok, but don't take time
     *              ENDIF
     *      ENDIF
     * ENDIF
     */

    /*
     * This is a local reference.
     */
    if (secEngineIDLen == myIDLength
        && memcmp(secEngineID, myID, myIDLength) == 0) {
        u_int           time_difference = myTime > time_uint ?
            myTime - time_uint : time_uint - myTime;

        if (boots_uint == ENGINEBOOT_MAX
            || boots_uint != myBoots
            || time_difference > USM_TIME_WINDOW) {
            snmp_increment_statistic(STAT_USMSTATSNOTINTIMEWINDOWS);

            DEBUGMSGTL(("usm",
                        "boot_uint %u myBoots %u time_diff %u => not in time window\n",
                        boots_uint, myBoots, time_difference));
            *error = SNMPERR_USM_NOTINTIMEWINDOW;
            return -1;
        }

        *error = SNMPERR_SUCCESS;
        return 0;
    }

    /*
     * This is a remote reference.
     */
    else {
        u_int           theirBoots, theirTime, theirLastTime;
        u_int           time_difference;

        if (get_enginetime_ex(secEngineID, secEngineIDLen,
                              &theirBoots, &theirTime,
                              &theirLastTime, TRUE)
            != SNMPERR_SUCCESS) {
            DEBUGMSGTL(("usm", "%s\n",
                        "Failed to get remote engine's times."));

            *error = SNMPERR_USM_GENERICERROR;
            return -1;
        }

        time_difference = theirTime > time_uint ?
            theirTime - time_uint : time_uint - theirTime;


        /*
         * XXX  Contrary to the pseudocode:
         *      See if boots is invalid first.
         */
        if (theirBoots == ENGINEBOOT_MAX || theirBoots > boots_uint) {
            DEBUGMSGTL(("usm", "%s\n", "Remote boot count invalid."));

            *error = SNMPERR_USM_NOTINTIMEWINDOW;
            return -1;
        }


        /*
         * Boots is ok, see if the boots is the same but the time
         * is old.
         */
        if (theirBoots == boots_uint && time_uint < theirLastTime) {
            if (time_difference > USM_TIME_WINDOW) {
                DEBUGMSGTL(("usm", "%s\n", "Message too old."));
                *error = SNMPERR_USM_NOTINTIMEWINDOW;
                return -1;
            }

            else {              /* Old, but acceptable */

                *error = SNMPERR_SUCCESS;
                return 0;
            }
        }


        /*
         * Message is ok, either boots has been advanced, or
         * time is greater than before with the same boots.
         */

        if (set_enginetime(secEngineID, secEngineIDLen,
                           boots_uint, time_uint, TRUE)
            != SNMPERR_SUCCESS) {
            DEBUGMSGTL(("usm", "%s\n",
                        "Failed updating remote boot/time."));
            *error = SNMPERR_USM_GENERICERROR;
            return -1;
        }

        *error = SNMPERR_SUCCESS;
        return 0;               /* Fresh message and time updated */

    }                           /* endif -- local or remote time reference. */


}                               /* end usm_check_and_update_timeliness() */