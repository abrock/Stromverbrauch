#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <SerialStream.h>

using namespace LibSerial ;

class LibSerialHelper {
public: 

int Open(SerialStream& serial_port, const char * SERIAL_PORT_DEVICE, int baudrate = 9600) const {
    //
    // Open the serial port.
    //
    serial_port.Open( SERIAL_PORT_DEVICE ) ;
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not open serial port "
                  << SERIAL_PORT_DEVICE
                  << std::endl ;
        exit(1) ;
    }
    //
    // Set the baud rate of the serial port.
    //
		switch (baudrate) {
			case 50: serial_port.SetBaudRate( SerialStreamBuf::BAUD_50 ); break;
			case 110: serial_port.SetBaudRate( SerialStreamBuf::BAUD_110 ); break;
			case 150: serial_port.SetBaudRate( SerialStreamBuf::BAUD_150 ); break;
			case 300: serial_port.SetBaudRate( SerialStreamBuf::BAUD_300 ); break;
			case 1200: serial_port.SetBaudRate( SerialStreamBuf::BAUD_1200 ); break;
			case 2400: serial_port.SetBaudRate( SerialStreamBuf::BAUD_2400 ); break;
			case 4800: serial_port.SetBaudRate( SerialStreamBuf::BAUD_4800 ); break;
			case 19200: serial_port.SetBaudRate( SerialStreamBuf::BAUD_19200 ); break;
			case 38400: serial_port.SetBaudRate( SerialStreamBuf::BAUD_38400 ); break;
			case 57600: serial_port.SetBaudRate( SerialStreamBuf::BAUD_57600 ); break;
			case 115200: serial_port.SetBaudRate( SerialStreamBuf::BAUD_115200 ); break;
			default: serial_port.SetBaudRate( SerialStreamBuf::BAUD_9600 ); break;
		}
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not set the baud rate." << std::endl ;
        exit(1) ;
    }
    //
    // Set the number of data bits.
    //
    serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not set the character size." << std::endl ;
        exit(1) ;
    }
    //
    // Disable parity.
    //
    serial_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not disable the parity." << std::endl ;
        exit(1) ;
    }
    //
    // Set the number of stop bits.
    //
    serial_port.SetNumOfStopBits( 1 ) ;
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not set the number of stop bits."
                  << std::endl ;
        exit(1) ;
    }
    //
    // Turn on hardware flow control.
    //
    serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
    if ( ! serial_port.good() )
    {
        std::cerr << "Error: Could not use hardware flow control."
                  << std::endl ;
        exit(1) ;
    }
   return 0;
}

};
