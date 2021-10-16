#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>

#ifndef WIN32
#include <signal.h>
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libirecovery.h>

irecv_client_t irecv_client = NULL;
irecv_device_t irecv_device = NULL;

extern "C" int enter_recovery()
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
    printf("Calling device to enter recovery..\n");
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

    return res;
}

extern "C" void reboot_idevice()
{
    printf("Rebooting device");
    irecv_setenv(irecv_client, "auto-boot", "true");
    irecv_saveenv(irecv_client);
    irecv_reboot(irecv_client);
    irecv_close(irecv_client);
}

extern "C" bool get_irecv_client()
{
    for (int i = 0; i <= 5; i++)
    {
        irecv_error_t err = irecv_open_with_ecid(&irecv_client, 0);
        if (err == IRECV_E_UNSUPPORTED)
        {
            return false;
        }
        else if (err != IRECV_E_SUCCESS)
            sleep(1);
        else
            break;

        if (i == 5)
        {
            return false;
        }
    }

    irecv_devices_get_device_by_client(irecv_client, &irecv_device);

    return true;
}

extern "C" const struct irecv_device_info *get_apnonce()
{
    const struct irecv_device_info *info = irecv_get_device_info(irecv_client);
    return info;
}