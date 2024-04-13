    int PamBackend::converse(int n, const struct pam_message **msg, struct pam_response **resp) {
        qDebug() << "[PAM] Conversation with" << n << "messages";

        bool newRequest = false;

        if (n <= 0 || n > PAM_MAX_NUM_MSG)
            return PAM_CONV_ERR;

        for (int i = 0; i < n; i++) {
            switch(msg[i]->msg_style) {
                case PAM_PROMPT_ECHO_OFF:
                case PAM_PROMPT_ECHO_ON:
                    newRequest = m_data->insertPrompt(msg[i], n == 1);
                    break;
                case PAM_ERROR_MSG:
                    m_app->error(QString::fromLocal8Bit(msg[i]->msg), Auth::ERROR_AUTHENTICATION);
                    break;
                case PAM_TEXT_INFO:
                    // if there's only the info message, let's predict the prompts too
                    m_app->info(QString::fromLocal8Bit(msg[i]->msg), m_data->handleInfo(msg[i], n == 1));
                    break;
                default:
                    break;
            }
        }

        if (newRequest) {
            Request sent = m_data->getRequest();
            Request received;

            if (sent.valid()) {
                received = m_app->request(sent);

                if (!received.valid())
                    return PAM_CONV_ERR;

                m_data->completeRequest(received);
            }
        }

        *resp = (struct pam_response *) calloc(n, sizeof(struct pam_response));
        if (!*resp) {
            return PAM_BUF_ERR;
        }

        for (int i = 0; i < n; i++) {
            QByteArray response = m_data->getResponse(msg[i]);

            resp[i]->resp = (char *) malloc(response.length() + 1);
            // on error, get rid of everything
            if (!resp[i]->resp) {
                for (int j = 0; j < n; j++) {
                    free(resp[i]->resp);
                    resp[i]->resp = nullptr;
                }
                free(*resp);
                *resp = nullptr;
                return PAM_BUF_ERR;
            }

            memcpy(resp[i]->resp, response.constData(), response.length());
            resp[i]->resp[response.length()] = '\0';
        }

        return PAM_SUCCESS;
    }