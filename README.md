# cmsis_g071rb

## Tool installation
```sh
sudo apt -y update
sudo apt -y install gcc-arm-none-eabi make stlink-tools git
```

## Verify installation
```sh
git clone https://github.com/cpq/bare-metal-programming-guide
cd bare-metal-programming-guide/steps/step-0-minimal
make
```

## Bit Operations

Clearing N bits at position X
```c
REGISTER &= ~((2^N - 1) << X);
```

Setting bit range to value
```C
REGISTER |= VALUE << X;
```

## GPIO Peripheral

Defining Pin
```C
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)
```

Setting Pin Mode
```C
static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  struct gpio *gpio = GPIO(PINBANK(pin)); // GPIO bank
  uint8_t n = PINNO(pin);                 // Pin number
  gpio->MODER &= ~(3U << (n * 2));        // Clear existing setting
  gpio->MODER |= (mode & 3) << (n * 2);   // Set new mode
}
```