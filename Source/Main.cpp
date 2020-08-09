#include <Windows.h>

extern "C" void __declspec(dllexport) hello()
{
}

template <typename T>
void patch(DWORD dwAddress, const T& data)
{
    DWORD p1, p2;
    LPVOID pAddress = reinterpret_cast<LPVOID>(dwAddress);

    VirtualProtect(pAddress, sizeof(T), PAGE_EXECUTE_READWRITE, &p1);
    *static_cast<T*>(pAddress) = data;
    VirtualProtect(pAddress, sizeof(T), p1, &p2);
}

// TickShift = 4 means 62.5 tps
// TickShift = 3 means 125 tps
// so on...

typedef unsigned char byte;

#define TickShift 3
#define TickMs    (1 << TickShift)
#define TickSec   (float(TickMs) / 1000.0f)
#define TickMask  (TickMs - 1)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason != DLL_PROCESS_ATTACH)
        return TRUE;

    // TickSec (0.032) patches
    // 0.032 float immediates and one in rdata

    const DWORD lTickSec[] =
    {
        0x4B8D30,
        0x4E9BAC,
        0x50F94B,
        0x51AA43,
        0x51AA4F,
        0x51AA5E,
        0x5320F2,
        0x532193,
        0x5426D6,
        0x5427A7,
        0x6D18F0 // rdata
    };

    for(int i = 0; i < sizeof(lTickSec) / sizeof(lTickSec[0]); i++)
        patch(lTickSec[i], TickSec);

    // TickMs (32) patches
    // 32 byte immediates and one 32.0 float in rdata

    const DWORD lTickMs[] =
    {
        0x515D63,
        0x5556C1,
        0x515B03,
        0x578220,
        0x51D841,
        0x533DEF,
        0x533E30,
        0x533E71,
        0x55DFB4,
        0x58276F,
        0x54CFD4
    };

    for(int i = 0; i < sizeof(lTickMs) / sizeof(lTickMs[0]); i++)
        patch(lTickMs[i], byte(TickMs));

    patch(0x6D8108, float(TickMs)); // rdata

    // (1 / TickMs) patch
    // Just one float in rdata

    patch(0x6C546C, 1.0f / float(TickMs)); // rdata

    // ~TickMask patches
    // ~31 byte immediates

    patch(0x515ADE, byte(~TickMask));
    patch(0x515BC4, byte(~TickMask));

    // TickMask patches
    // 31 byte immediates

    patch(0x515BC1, byte(TickMask));
    patch(0x515C4E, byte(TickMask));

    // TickShift patches
    // 5 byte immediates

    patch(0x515BCD, byte(TickShift));
    patch(0x515AE5, byte(TickShift));

    // 9.81 * TickSec patch
    // One float in rdata

    patch(0x6D81BC, 9.81f * TickSec);

    // 0.5 * TickSec patch
    // One float in rdata

    patch(0x6D4AF0, 0.5f * TickSec);

    // 1.1 * TickSec patch
    // One float in rdata

    patch(0x6D6F2C, 1.1f * TickSec);

    // 10 * TickSec patch
    // One float in rdata

    patch(0x6E1168, 10.0f * TickSec);

    // Patch the max packet rate clamp

    patch(0x5C9498, 1024 / int(TickMs));

    // For projectile max lifetime, armingDelay, fadeDelay (ms is converted to ticks)

    patch(0x533DE8, 32736 / int(TickMs));
    patch(0x533E29, 32736 / int(TickMs));
    patch(0x533E6A, 32736 / int(TickMs));
        
    // Some areas of the game (Player::*packUpdate, Projectile::*packUpdate)
    // use the value 32.0 and 0.03125 for other uses, so fix them up.
    // We use an unused area of memory so we can rewrite what those use to point to.

    // Set unused areas to 32.0 and 0.03125

    patch(0x6DF560, 32.0f);
    patch(0x6DF564, 0.03125f);

    // Now re-point to those addresses.

    patch(0x535CA6, 0x6DF564);
    patch(0x5314A7, 0x6DF564);
    patch(0x535A7E, 0x6DF560);

    return TRUE;
}
