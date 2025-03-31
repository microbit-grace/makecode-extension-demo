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

    /**
    * Send the data to serial or BLE UART.
    * @param uart BLE uart service to send to. NULL to send to serial.
    */
    void log_sendToUart(const void *data, int len)
    {
        bool useUART = uart && uart->getConnected();

        if (useUART)
            uart->send( (const uint8_t *) data, len);
        else
            uBit.serial.send( (uint8_t *) data, len);
    }

    /**
    * Send the logged data to serial or BLE UART.
    * @param uart BLE uart service to send to. NULL to send to serial.
    * @param format Which data to send
    */
    int log_sendDataToUart(DataFormat format)
    {
        ManagedBuffer buffer(CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE);

        uint32_t length = uBit.log.getDataLength( format);
        uint32_t index  = 0;
        uint32_t remain = length;
        
        int result = DEVICE_OK;

        while (remain)
        {
            uint32_t block = min(buffer.length(), remain);

            result = uBit.log.readData( &buffer[0], index, block, format, length);
            if (result != DEVICE_OK)
            {
                break;
            }
            
            log_sendToUart( &buffer[0], block);

            index += block;
            remain -= block;
        }
        
        return result;
    }
    
    /**
    *  Logs read data
    */
    //% blockId=cpp_log_read_data block="cpp log read data"
    void logReadData() {
        int r = log_sendDataToUart(DataFormat::CSV);
    }
}