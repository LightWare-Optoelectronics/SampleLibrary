//---------------------------------------------------------
// LightWare Raspberry Pi2 I2C Connection Sample
//---------------------------------------------------------

// Compile with:
// gcc -O3 -DRPI rpi_i2c_c.c -o rpi_i2c_c -lwiringPi

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

int main(int argc, char **argv)
{
    int devieAddress = 0x55;
	int fd = wiringPiI2CSetup(devieAddress);
    
    if (fd == -1)
    {
        printf("I2C Bus file could not be opened\n");
    }
    else
    {
		printf("I2C Bus opened on FD: %d\n", fd);
        
        while (1)
        {   
            unsigned char byte[2];
            int res = read(fd, byte, 2);

            if (res == -1)
            {
                printf("I2C Device with address %d was not available\n", devieAddress);
            }
            else
            {
                int distanceInCM = (byte[0] << 8) | byte[1];
                printf("Distance: %dcm\n", distanceInCM);
            }
            
            delay(250);
        }    
    }

    return 0;
}