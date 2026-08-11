#include <stdio.h>
#include "gd32f5xx_gpio.h"
#include "hal_i2c.h"

#define I2C0_SLAVE_ADDRESS7 ((uint8_t)0xA0)
#define I2C0_SPEED          ((uint32_t)100000)

void i2c0_gpio_init(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* connect I2C_SCL_PIN to I2C_SCL */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_8);
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_9);
    /* configure GPIO pins of I2C */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
}
void i2c0_init(void)
{
    rcu_periph_clock_enable(RCU_I2C0);
    /* configure I2C clock */
    i2c_clock_config(I2C0, I2C0_SPEED, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

void i2c0_deinit(void)
{
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
}

static void i2c_read(uint32_t i2c_periph, uint8_t addr, uint8_t read_address, uint8_t* p_buffer, uint16_t number_of_byte)
{  
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));
    if(2 == number_of_byte) {
        i2c_ackpos_config(i2c_periph, I2C_ACKPOS_NEXT);
    }
    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, addr << 1, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear the ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while(SET != i2c_flag_get(i2c_periph, I2C_FLAG_TBE));
    /* enable I2C*/
    i2c_enable(i2c_periph);
    /* send the EEPROM's internal address to write to */
    i2c_data_transmit(i2c_periph, read_address);  
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, addr << 1, I2C_RECEIVER);
    if(number_of_byte < 3) {
        /* disable acknowledge */
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    }
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear the ADDSEND bit */
    i2c_flag_clear(i2c_periph,I2C_FLAG_ADDSEND);
    if(1 == number_of_byte) {
        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(i2c_periph);
    }
    
    /* while there is data to be read */
    while(number_of_byte) {
        if(3 == number_of_byte) {
            /* wait until BTC bit is set */
            while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
            /* disable acknowledge */
            i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
        }
        if(2 == number_of_byte) {
            /* wait until BTC bit is set */
            while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(i2c_periph);
        }
        
        /* wait until the RBNE bit is set and clear it */
        if(i2c_flag_get(i2c_periph, I2C_FLAG_RBNE)) {
            /* read a byte from the EEPROM */
            *p_buffer = i2c_data_receive(i2c_periph);
            /* point to the next location where the byte read will be saved */
            p_buffer++; 
            /* decrement the read bytes counter */
            number_of_byte--;
        } 
    }
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(i2c_periph)&0x0200);
    /* enable acknowledge */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
    i2c_ackpos_config(i2c_periph, I2C_ACKPOS_CURRENT);
}

static void i2c_write(uint32_t i2c_periph, uint8_t addr, uint8_t write_address, uint8_t *p_buffer, uint8_t number_of_byte)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, addr << 1, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear the ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while(SET != i2c_flag_get(i2c_periph, I2C_FLAG_TBE));
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(i2c_periph, write_address);
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));

    /* while there is data to be written */
    while(number_of_byte--) {  
        i2c_data_transmit(i2c_periph, *p_buffer);
        /* point to the next byte to be written */
        p_buffer++; 
        /* wait until BTC bit is set */
        while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
    }
    i2c_stop_on_bus(i2c_periph);

    /* wait until the stop condition is finished */
    while(I2C_CTL0(i2c_periph)&0x0200); 
    /* enable acknowledge */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
    i2c_ackpos_config(i2c_periph, I2C_ACKPOS_CURRENT);
}

typedef enum {
	I2C_START = 0,
	I2C_SEND_ADDRESS,
	I2C_CLEAR_ADDRESS_FLAG,
	I2C_TRANSMIT_DATA,
	I2C_STOP,
	I2C_FAIL_STOP
} i2c_process_enum;

#define I2C_TIME_OUT    (uint16_t)(5000)

static void hal_i2c_reset(uint32_t i2c_periph)
{
    switch (i2c_periph) {
    case I2C0:
        i2c_deinit(i2c_periph);
        /* configure SDA/SCL for GPIO */
        GPIO_BC(GPIOB) |= GPIO_PIN_8;
        GPIO_BC(GPIOB) |= GPIO_PIN_9;
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        GPIO_BOP(GPIOB) |= GPIO_PIN_8;
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        GPIO_BOP(GPIOB) |= GPIO_PIN_9;
        /* connect I2C_SCL_PIN to I2C_SCL */
        /* connect I2C_SDA_PIN to I2C_SDA */
        gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
        /* configure the I2CX interface */
        i2c0_init();
        break;
    default:
        break;
    }
}

uint8_t hal_check_i2c_periph_ready(uint32_t i2c_periph, uint8_t addr)
{
    uint8_t state = I2C_START;
    uint16_t timeout = 0;
    uint8_t i2c_timeout_flag = 0;

    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(i2c_periph);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                hal_i2c_reset(i2c_periph);
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in EEPROM standby!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_master_addressing(i2c_periph, addr << 1, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends start signal timeout in EEPROM standby!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            while((!((i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND)) || (i2c_flag_get(i2c_periph, I2C_FLAG_AERR)))) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                if(i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
                    timeout = 0;
                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(i2c_periph);
                    i2c_timeout_flag = I2C_RW_SUCCESS;
                    /* exit the function */
                    return I2C_RW_SUCCESS;
                } else {
                    /* clear the bit of AERR */
                    i2c_flag_clear(i2c_periph, I2C_FLAG_AERR);
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master clears address flag timeout in EEPROM standby!\n");
            }
            break;
        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(i2c_periph);
            return I2C_RW_FAIL;
            break;
        default:
            state = I2C_START;
            timeout = 0;
            printf("i2c master sends start signal end in EEPROM standby!\n");
            break;
        }
    }
    return I2C_RW_SUCCESS;
}

uint8_t hal_i2c_read(enum I2C_ID i2c_id, uint32_t addr, uint8_t read_address, uint8_t *data, uint16_t len)
{
	uint32_t i2c_periph;
    switch(i2c_id) {
    case I2C_ID_0:
        i2c_periph = I2C0;
        break;
    }

    if(hal_check_i2c_periph_ready(i2c_periph, addr) == I2C_RW_FAIL) {
        return I2C_RW_FAIL;
    }
    i2c_read(i2c_periph, addr, read_address, data, len);
    return I2C_RW_SUCCESS;
}

uint8_t hal_i2c_write(enum I2C_ID i2c_id, uint32_t addr, uint8_t write_address, uint8_t *data, uint16_t len)
{
	uint32_t i2c_periph;
    switch(i2c_id) {
    case I2C_ID_0:
        i2c_periph = I2C0;
        break;
    }

    if(hal_check_i2c_periph_ready(i2c_periph, addr) == I2C_RW_FAIL) {
        return I2C_RW_FAIL;
    }
	i2c_write(i2c_periph, addr, write_address, data, len);
	return I2C_RW_SUCCESS;
}

void hal_i2c_init(void)
{
    i2c0_gpio_init();
    i2c0_init();
}