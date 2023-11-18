/* Author: Generated with chatgpt and modified by Andrea Ongarini */

#include <bcm2835.h>
#include <stdio.h>

#define DHT_PIN RPI_GPIO_P1_07  // GPIO pin for DHT11

int read_dht11(uint8_t *temperature, uint8_t *humidity) {
    
    uint8_t data[5] = {0, 0, 0, 0, 0}; // 5 Bytes from DHT11

	// Send start signal to DHT11
    
    /* RPi sends 18 ms LOW*/
    bcm2835_gpio_fsel(DHT_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(DHT_PIN, LOW);
    bcm2835_delay(18);
    
    /* RPi sends 40 us HIGH*/
    bcm2835_gpio_write(DHT_PIN, HIGH);
    bcm2835_gpio_fsel(DHT_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(40);

    // Read data from DHT11

    /* DHT11 sends 80 us HIGH */
    if (bcm2835_gpio_lev(DHT_PIN) == HIGH) {
        return -1; // DHT11 response error
    }
    bcm2835_delayMicroseconds(80);
    
    /* DHT11 sends 80 us LOW */
    if (bcm2835_gpio_lev(DHT_PIN) == LOW) {
        return -1; // DHT11 response error
    }
    bcm2835_delayMicroseconds(80);
    
    // Read 40 bits of data
    for (int i = 0; i < 40; ++i) {
        while (bcm2835_gpio_lev(DHT_PIN) == LOW); // "dont't care" about LOW time
        
        bcm2835_delayMicroseconds(28);
        
        if (bcm2835_gpio_lev(DHT_PIN) == HIGH) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
        
        while (bcm2835_gpio_lev(DHT_PIN) == HIGH);
    }

    // Verify checksum
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return -1; // Checksum error
    }
    
    // DHT11 doesn't measue decimals!
    *humidity = data[0];
    *temperature = data[2];

    return 0; // Success
}

int main() {
    
    if (!bcm2835_init()) {
        printf("bcm2835_init failed. Are you running as root?\n");
        return 1;
    }

    uint8_t temperature, humidity;

	for(;;){
		if (read_dht11(&temperature, &humidity) == 0) {
			printf("Temperature = %d°C, Humidity = %d%%\n", temperature, humidity);
		} else {
			printf("Failed to read data from DHT11\n");
		}
		bcm2835_delay(1500);
	}
	
    bcm2835_close();
    return 0;
}
    
