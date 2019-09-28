#include <string.h>
#include "netstack.h"

bool mac_equals(mac_t a, mac_t b) {
    return memcmp(a.data, b.data, 6) == 0;
}
