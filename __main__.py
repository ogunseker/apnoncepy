import ctypes
from datetime import datetime, timedelta
from time import time


class irecv_device_info(ctypes.Structure):
    _fields_ = [
        ("cpid", ctypes.c_uint),
        ("cprv", ctypes.c_uint),
        ("cpfm", ctypes.c_uint),
        ("scep", ctypes.c_uint),
        ("bdid", ctypes.c_uint),
        ("ecid", ctypes.c_uint64),
        ("ibfl", ctypes.POINTER(ctypes.c_uint)),
        ("srnm", ctypes.POINTER(ctypes.c_char)),
        ("imei", ctypes.POINTER(ctypes.c_char)),
        ("srtg", ctypes.POINTER(ctypes.c_char)),
        ("serial_string", ctypes.POINTER(ctypes.c_char)),
        ("ap_nonce", ctypes.POINTER(ctypes.c_ubyte)),
        ("ap_nonce_size", ctypes.c_uint),
        ("sep_nonce", ctypes.POINTER(ctypes.c_ubyte)),
        ("sep_nonce_size", ctypes.c_uint),
    ]


if __name__ == "__main__":
    lib = ctypes.CDLL("libpayload.so")
    lib.get_irecv_client.restype = ctypes.c_bool
    lib.enter_recovery.restype = ctypes.c_int
    lib.get_apnonce.restype = ctypes.POINTER(irecv_device_info)

    recovery_status = lib.enter_recovery()

    deadline = datetime.now() + timedelta(seconds=60)
    err = False

    while datetime.now() < deadline and err == False:
        try:
            err = lib.get_irecv_client()
        except Exception:
            print("Error occured")

    device_info = lib.get_apnonce()
    ap_nonce = device_info.contents.ap_nonce
    buf_size = device_info.contents.ap_nonce_size

    print("Your APNonce is: ")
    print(
        "".join(
            [
                format(i, "02x")
                for i in ctypes.cast(
                    ap_nonce, ctypes.POINTER((ctypes.c_ubyte * buf_size))
                ).contents
            ]
        )
    )

    lib.reboot_idevice()
