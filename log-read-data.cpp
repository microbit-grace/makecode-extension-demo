// log-read-data.cpp
// Test MicroBitLog::readData
// Press A to toggle logging
// Press B to retrive data and send files to UART:
//      - Empty HTML (header.htm)
//      - HTML with data (html.htm)
//      - CSV only (csv.csv)
//
// The UART will be BLE if connected, otherwise USB serial
// The python client "log-read-data.py" can read USB serial and save files
#include "pxt.h"
#include "MicroBitUARTService.h"
#include "MicroBitUtilityService.h"
#include "MicroBitLog.h"
#include "MicroBit.h"

using namespace pxt;

namespace logReadData
{
    MicroBit uBit;
    MicroBitUARTService *bleUART = NULL;
    bool log_read_data_running = false;

    /**
     * Send the data to serial or BLE UART.
     * @param uart BLE uart service to send to. NULL to send to serial.
     */
    void log_sendToUart(const void *data, int len)
    {
        bool useUART = bleUART && bleUART->getConnected();

        if (useUART)
            bleUART->send((const uint8_t *)data, len);
        else
            uBit.serial.send((uint8_t *)data, len);
    }

    void log_sendToUart(const char *data)
    {
        bool useUART = bleUART && bleUART->getConnected();

        if (useUART)
            bleUART->send((const uint8_t *)data, strlen(data));
        else
            uBit.serial.send((uint8_t *)data, strlen(data));
    }

    /**
     * Send the logged data to BLE UART.
     * @param uart BLE uart service to send to. NULL to send to serial.
     * @param format Which data to send
     */
    int log_sendDataToUart(DataFormat format)
    {
        ManagedBuffer buffer(CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE);

        uint32_t length = uBit.log.getDataLength(format);
        uint32_t index = 0;
        uint32_t remain = length;

        int result = DEVICE_OK;

        while (remain)
        {
            uint32_t block = min(buffer.length(), remain);

            result = uBit.log.readData(&buffer[0], index, block, format, length);
            if (result != DEVICE_OK)
            {
                break;
            }

            log_sendToUart(&buffer[0], block);

            index += block;
            remain -= block;
        }

        return result;
    }

    void log_read_data_format(DataFormat format, ManagedString name)
    {
        ManagedString tag = "<<FILE=" + name + ">>";
        log_sendToUart(tag.toCharArray());

        int r = log_sendDataToUart(format);
        if (r == DEVICE_OK)
            log_sendToUart("<<FILEEND>>");
        else
            log_sendToUart("<<FILEERROR>>");
    }

    void log_read_data()
    {
        log_read_data_running = true;

        log_sendToUart("\r\n\r\n");

        log_read_data_format(DataFormat::HTMLHeader, "log_read_data_empty.htm");

        log_sendToUart("\r\n\r\n");

        log_read_data_format(DataFormat::HTML, "log_read_data.htm");

        log_sendToUart("\r\n\r\n");

        log_read_data_format(DataFormat::CSV, "log_read_data.csv");

        log_sendToUart("\r\n\r\n");

        log_sendToUart("<<STOP>>");

        log_sendToUart("\r\n\r\n");

        log_read_data_running = false;
        release_fiber();
    }

    /**
     *  Test
     */
    //% blockId=log_read_data block="log read data"
    void test()
    {
        MicroBitUtilityService::createShared(*uBit.ble, uBit.messageBus, uBit.storage, uBit.log);
        bleUART = new MicroBitUARTService(*uBit.ble, 240, 240);
        if (!log_read_data_running)
        {
            log_read_data_running = true;
            create_fiber(log_read_data);
        }
    }
}