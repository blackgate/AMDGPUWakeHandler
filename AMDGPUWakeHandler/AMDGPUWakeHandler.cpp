// Based on the linux implementationn of apple-gmux
// https://github.com/torvalds/linux/blob/master/drivers/platform/x86/apple-gmux.c

#include <IOKit/IOLib.h>
#include "AMDGPUWakeHandler.hpp"

#define kMyNumberOfStates 2
#define kIOPMPowerOff 0

#define GMUX_IOSTART                0x700

#define GMUX_PORT_SWITCH_DISPLAY    0x10
#define GMUX_PORT_SWITCH_DDC        0x28
#define GMUX_PORT_SWITCH_EXTERNAL   0x40
#define GMUX_PORT_DISCRETE_POWER    0x50
#define GMUX_PORT_INTERRUPT_ENABLE  0x14

#define GMUX_INTERRUPT_ENABLE       0xff

#define GMUX_SWITCH_DDC_IGD         0x1
#define GMUX_SWITCH_DISPLAY_IGD     0x2
#define GMUX_SWITCH_EXTERNAL_IGD    0x2

#define GMUX_DISCRETE_POWER_OFF     0x0

// This required macro defines the class's constructors, destructors,
// and several other methods I/O Kit requires.
OSDefineMetaClassAndStructors(AMDGPUWakeHandler, IOService)

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void gmux_write8(uint16_t port, uint8_t val) {
    outb(GMUX_IOSTART + port, val);
}

uint8_t gmux_read8(unsigned short port) {
    return inb(GMUX_IOSTART + port);
}

uint8_t get_discrete_state() {
    return gmux_read8(GMUX_PORT_DISCRETE_POWER);
}

// Define the driver's superclass.
#define super IOService

bool AMDGPUWakeHandler::init(OSDictionary *dict)
{
    bool result = super::init(dict);
    IOLog("Initializing\n");
    return result;
}

void AMDGPUWakeHandler::free(void)
{
    IOLog("Freeing\n");
    super::free();
}

IOService *AMDGPUWakeHandler::probe(IOService *provider,
                                    SInt32 *score)
{
    IOService *result = super::probe(provider, score);
    IOLog("Probing\n");
    if (get_discrete_state() != 0) {
        IOLog("Failed to Load. Discrete GPU was powered on\n");
        return 0;
    }
    return result;
}

bool AMDGPUWakeHandler::start(IOService *provider)
{
    bool result = super::start(provider);
    IOLog("Starting\n");
    PMinit();
    provider->joinPMtree(this);
    static IOPMPowerState myPowerStates[kMyNumberOfStates] = {
        {1, kIOPMPowerOff, kIOPMPowerOff, kIOPMPowerOff, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    registerPowerDriver (this, myPowerStates, kMyNumberOfStates);

    return result;
}

void AMDGPUWakeHandler::stop(IOService *provider)
{
    IOLog("Stopping\n");
    PMstop();
    super::stop(provider);
}

void AMDGPUWakeHandler::disableGPU()
{
    IOLog("Disabling GPU\n");
    gmux_write8(GMUX_PORT_INTERRUPT_ENABLE, GMUX_INTERRUPT_ENABLE);
    gmux_write8(GMUX_PORT_SWITCH_DDC, GMUX_SWITCH_DDC_IGD);
    gmux_write8(GMUX_PORT_SWITCH_DISPLAY, GMUX_SWITCH_DISPLAY_IGD);
    gmux_write8(GMUX_PORT_SWITCH_EXTERNAL, GMUX_SWITCH_EXTERNAL_IGD);
    gmux_write8(GMUX_PORT_DISCRETE_POWER, GMUX_DISCRETE_POWER_OFF);
}

IOReturn AMDGPUWakeHandler::setPowerState ( unsigned long whichState, IOService * whatDevice )
{
    if ( kIOPMPowerOff != whichState ) {
        IOLog("Waking up\n");
        this->disableGPU();
    }
    return kIOPMAckImplied;
}
