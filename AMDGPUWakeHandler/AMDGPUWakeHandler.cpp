
#include <IOKit/IOLib.h>
#include "AMDGPUWakeHandler.hpp"

#define kMyNumberOfStates 2
#define kIOPMPowerOff 0

// This required macro defines the class's constructors, destructors,
// and several other methods I/O Kit requires.
OSDefineMetaClassAndStructors(AMDGPUWakeHandler, IOService)

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (value));
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
    outb(0x728, 0x1);
    outb(0x710, 0x2);
    outb(0x740, 0x2);
    outb(0x750, 0x0);
}

IOReturn AMDGPUWakeHandler::setPowerState ( unsigned long whichState, IOService * whatDevice )
{
    if ( kIOPMPowerOff != whichState ) {
        IOLog("Waking up\n");
        this->disableGPU();
    }
    return kIOPMAckImplied;
}
