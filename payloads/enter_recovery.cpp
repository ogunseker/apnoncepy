#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifndef WIN32
#include <signal.h>
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

int main(int argc, char *argv[])
{
    lockdownd_client_t client = NULL;
    lockdownd_error_t ldret = LOCKDOWN_E_UNKNOWN_ERROR;
    idevice_t device = NULL;
    idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
    const char *udid = NULL;

#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif

    ret = idevice_new_with_options(&device, udid, IDEVICE_LOOKUP_USBMUX);
    if (ret != IDEVICE_E_SUCCESS)
    {
        if (udid)
        {
            printf("ERROR: Device %s not found!\n", udid);
        }
        else
        {
            printf("ERROR: No device found!\n");
        }
        return -1;
    }

    if (LOCKDOWN_E_SUCCESS != (ldret = lockdownd_client_new_with_handshake(device, &client, "apnoncepy")))
    {
        fprintf(stderr, "ERROR: Could not connect to lockdownd: %s (%d)\n", lockdownd_strerror(ldret), ldret);
        idevice_free(device);
        return -1;
    }

    int res = 0;
    printf("Calling device to enter recovery\n");
    ldret = lockdownd_enter_recovery(client);
    if (ldret == LOCKDOWN_E_SESSION_INACTIVE)
    {
        lockdownd_client_free(client);
        client = NULL;
        if (LOCKDOWN_E_SUCCESS != (ldret = lockdownd_client_new_with_handshake(device, &client, "TOOL_NAME")))
        {
            printf("ERROR: Could not connect to lockdownd: %s (%d)\n", lockdownd_strerror(ldret), ldret);
            idevice_free(device);
            return 1;
        }
        ldret = lockdownd_enter_recovery(client);
    }
    if (ldret != LOCKDOWN_E_SUCCESS)
    {
        printf("Failed to enter recovery mode.\n");
        res = 1;
    }
    else
    {
        printf("Device is successfully switching to recovery mode.\n");
    }
}