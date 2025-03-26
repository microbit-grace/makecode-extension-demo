#include "pxt.h"
#include "MESEvents.h"
#include "MicroBitUARTService.h"
#include "MicroBitUtilityService.h"

using namespace pxt;

//% color=#0082FB weight=96 icon="\uf294"
namespace demo {
    MicroBitUARTService *uart = NULL;
    MicroBitUtilityService *utility = NULL;

    /**
    *  Starts the Bluetooth UART service
    */
    //% blockId=bluetooth_start_uart_service block="bluetooth uart service"
    void startUartService() {
        if (uart) return;
        // 61 octet buffer size is 3 x (MTU - 3) + 1
        // MTU on nRF51822 is 23 octets. 3 are used by Attribute Protocol header data leaving 20 octets for payload
        // So we allow a RX buffer that can contain 3 x max length messages plus one octet for a terminator character
        uart = new MicroBitUARTService(*uBit.ble, 61, 60);
    }

    /**
    *  Starts the Bluetooth Utility service
    */
    //% blockId=bluetooth_start_utility_service block="bluetooth utility service"
    void startUtilityService() {
        MicroBitUtilityService::createShared( *uBit.ble, uBit.messageBus, uBit.storage, uBit.log);
    }
}