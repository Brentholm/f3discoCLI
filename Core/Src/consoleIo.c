// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
#include <stdio.h>
#include "usart.h"

//use the windows conio.h for kbhit, or a POSIX reproduction
#ifdef _WIN32
#include <conio.h>
#else
#include "conioCompat.h"
#endif

//their original
/*static int getch_noblock() {
    if (_kbhit())
        return _getch();
    else
        return EOF;
}*/

//my HAL version to do the same:
//int getch_noblock(UART_HandleTypeDef *huart) {
int getch_noblock(void) {
    uint8_t receivedChar;

    if (HAL_UART_Receive(&huart1, &receivedChar, 1, 1) == HAL_OK) {
        //echo it out
    	HAL_UART_Transmit(&huart1,&receivedChar, 1, 1);
    	// A character has been received and stored in 'receivedChar'

        return receivedChar;
    } else {
        // No character received within the timeout period
        return 0xFFFFFFFF;  //this seems to match the EOF that ConsoleIOReceive is looking for.
    }
}

eConsoleError ConsoleIoInit(void)
{
	return CONSOLE_SUCCESS;
}
/*eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint32_t i = 0;
	char ch;
	
	ch = getch_noblock();
	while ( ( EOF != (uint8_t)ch ) && ( i < bufferLength ) )
	{
		buffer[i] = (uint8_t) ch;
		i++;
		ch = getch_noblock();
	}
	*readLength = i;
	return CONSOLE_SUCCESS;
}*/

eConsoleError ConsoleIoReceive(uint8_t* buffer, const uint32_t bufferLength, uint32_t *readLength)
{
    uint32_t i = 0;
    uint8_t rxByte = 0;


 while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);

 if (HAL_OK != HAL_UART_Receive(&huart1, &rxByte, 1, HAL_MAX_DELAY))
 {
  return CONSOLE_ERROR;
 }

 /* Send echo */
 HAL_UART_Transmit(&huart1, (uint8_t*)&rxByte, 1, HAL_MAX_DELAY);

 buffer[i] = rxByte;
 i++;


 *readLength = i;

 return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
	printf("%s", buffer);
	return CONSOLE_SUCCESS;
}

