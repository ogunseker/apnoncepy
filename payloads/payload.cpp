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

int reboot_idevice()
{
    irecv_setenv(irecv_client, "auto-boot", "true");
    irecv_saveenv(irecv_client);
    irecv_reboot(irecv_client);
    irecv_close(irecv_client);
}

bool get_irecv_client()
{
    for (int i = 0; i <= 5; i++)
    {
        irecv_error_t err = irecv_open_with_ecid(&irecv_client, 0);
        if (err == IRECV_E_UNSUPPORTED)
        {
            fprintf(stderr, "ERROR: %s\n", irecv_strerror(err));
            return false;
        }
        else if (err != IRECV_E_SUCCESS)
            sleep(1);
        else
            break;

        if (i == 5)
        {
            fprintf(stderr, "ERROR: %s\n", irecv_strerror(err));
            return false;
        }
    }

    irecv_devices_get_device_by_client(irecv_client, &irecv_device);

    return true;
}

int main(int argc, char *argv[])
{
    get_irecv_client();
    reboot_idevice();
}