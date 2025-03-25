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
#include "MESEvents.h"
#include "MicroBit.h"

using namespace pxt;

namespace logReadData
{
    MicroBit uBit;

    MicroBitUARTService *bleUART = NULL;

    ////////////////////////////////////////////////////////////////

    int num_cols = 10; // The number of additional dummy columns (0 to 26)
    int period = 100;  // The logging loop period in ms (0 for minimum delay between iterations)

    int loops = 0;
    int logging = 0;

    void datalogFull(MicroBitEvent e)
    {
        if (logging != 0)
        {
            logging = 0;
            loops++;
            uBit.serial.send("FULL " + ManagedString(loops) + "\n");
            uBit.display.printChar('F');
            uBit.audio.soundExpressions.play("giggle");
            uBit.sleep(1000);
            uBit.log.clear(false);
            uBit.display.scroll(loops);
            uBit.sleep(1000);
            logging = 1;
        }
    }

    void log()
    {
        const char *head = "a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z\0";

        double delta = 0;
        double count = 0;

        uBit.log.clear(false);
        uBit.log.setSerialMirroring(false);
        uBit.log.setTimeStamp(TimeStampFormat::Milliseconds);

        uBit.messageBus.listen(MICROBIT_ID_LOG, MICROBIT_LOG_EVT_LOG_FULL, datalogFull);

        while (true)
        {
            if (!logging)
            {
                uBit.sleep(100);
                continue;
            }

            CODAL_TIMESTAMP loop0 = system_timer_current_time();

            uBit.display.image.setPixelValue(0, 0, uBit.display.image.getPixelValue(0, 0) ? 0 : 255);

            uBit.log.beginRow();

            uBit.log.logData("delta", ManagedString((int)delta));
            uBit.log.logData("txbuf", ManagedString((int)uBit.serial.txBufferedSize()));

            for (int col = 0; col < num_cols; col++)
            {
                uBit.log.logData(head[col * 2], "1023");
            }

            double now0 = system_timer_current_time();
            uBit.log.endRow();
            double now1 = system_timer_current_time();

            count += 1;
            delta = now1 - now0;

            CODAL_TIMESTAMP loop1 = system_timer_current_time();
            int loopTime = loop1 - loop0;
            int wait = period - loopTime;
            if (wait < 0)
                wait = 0;
            uBit.sleep(wait);
        }
    }

    ////////////////////////////////////////////////////////////////

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
     * Send the logged data to serial or BLE UART.
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

    void display()
    {
        while (true)
        {
            while (!logging && !log_read_data_running)
            {
                uBit.display.scroll("A=TOGGLE LOGGING B=SEND", 100);
                uBit.sleep(500);
            }

            uBit.display.clear();

            while (logging || log_read_data_running)
            {
                if (log_read_data_running)
                    uBit.display.image.setPixelValue(4, 4, uBit.display.image.getPixelValue(4, 4) ? 0 : 255);

                uBit.sleep(500);
            }
        }
    }

    void onClickA(MicroBitEvent e)
    {
        logging = !logging;
    }

    void onClickB(MicroBitEvent e)
    {
        if (!log_read_data_running)
        {
            log_read_data_running = true;
            create_fiber(log_read_data);
        }
    }

    /**
     *  Main
     */
    //% blockId=log_read_data block="log read data"
    int main()
    {
        uBit.init();

        uBit.serial.setRxBufferSize(254);
        uBit.serial.setTxBufferSize(254);

        bleUART = new MicroBitUARTService(*uBit.ble, 240, 240);

        uBit.messageBus.listen(DEVICE_ID_BUTTON_A, DEVICE_BUTTON_EVT_CLICK, onClickA);
        uBit.messageBus.listen(DEVICE_ID_BUTTON_B, DEVICE_BUTTON_EVT_CLICK, onClickB);

        create_fiber(display);
        create_fiber(log);
        release_fiber();
    }
}