#include "Vars.h"

using namespace std;
using namespace Proud;

// Protocol version that you define.
// Your server app and client app must have the same value below.
PNGUID guid = {0x489fa1cc, 0x5df3, 0x4581, 0x96, 0x56, 0x4d,
               0x71,       0xae,   0xc8,   0x34, 0xf1};

Guid g_Version = Guid(guid);

// TCP listening port number.
int g_ServerPort = 33334;
