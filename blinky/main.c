/* Libraries */
#include <inttypes.h>
#include <stdbool.h>

/* Defining a GPIO pin */
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

/* RCC Structure */
// Re-written to match G071
// See RM0444 Rev 6 Section 5.4.25 for reference to register map
struct rcc {
  volatile uint32_t CR, ICSCR, CFGR, PLLCFGR, RESERVED0, CRRCR, CIER, CIFR, CICR,
                    IOPRSTR, AHBRSTR, APBRSTR1, APBRSTR2, IOPENR, AHBENR, APBENR1,
                    APBENR2, IOPSMENR, AHBSMENR, APBSMENR1, APBSMENR2, CCIPR, CCIPR2
                    BDCR, CSR;
};
#define RCC ((struct rcc *) 0x40021000) // Base address for G071

struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
#define GPIO(bank) ((struct gpio *) (0x50000000 + 0x400 * (bank)))

// Enum values are per datasheet: 0, 1, 2, 3
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

/* Setting GPIO pin mode */
static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  struct gpio *gpio = GPIO(PINBANK(pin));  // GPIO bank
  int n = PINNO(pin);                      // Pin number
  gpio->MODER &= ~(3U << (n * 2));         // Clear existing setting
  gpio->MODER |= (mode & 3) << (n * 2);    // Set new mode
}

/* Set GPIO pin to HIGH or to LOW */
static inline void gpio_write(uint16_t pin, bool val) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

/* Basic delay function */
static inline void spin(volatile uint32_t count) {
  while (count--) (void) 0; // executes a NOP instruction a given number of times
}

/* Main Program */
// Gets called after the reset handler has initialized the system.
int main(void) {

  /* Program Setup */
  uint16_t led = PIN('A', 5);            // USER LED 
  RCC->IOPENR |= BIT(PINBANK(led));     // Enable GPIO clock for LED
  gpio_set_mode(led, GPIO_MODE_OUTPUT);  // Set blue LED to output mode
  
  /* Main Program Loop */
  for (;;) {
    gpio_write(led, true);
    spin(999999);
    gpio_write(led, false);
    spin(999999);
  }
  return 0; // Do nothing so far
}

/* Startup Code */
// Reset handler
__attribute__((naked, noreturn)) void _reset(void) { // Entry point for the linker script
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop in the case if main() returns
}

extern void _estack(void);  // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = { // defines the interrupt vector table and dumps it into .vectors via the linker script
    _estack, _reset};
