// zzxx
#include "clk.h"
#include "qm_gpio.h"
#include "qm_pinmux.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define LED_BOTTOM_LEFT		6
#define LED_BOTTOM_MIDDLE	0
#define LED_BOTTOM_RIGHT	1
#define LED_EYE_LEFT		18
#define LED_EYE_RIGHT		2
#define LED_ENABLE			9

#define BTN_L_UP			14
#define BTN_L_DOWN			17
#define BTN_L_LEFT			16
#define BTN_L_RIGHT			15
#define BTN_R_UP			11
#define BTN_R_DOWN			7
#define BTN_R_LEFT			10
#define BTN_R_RIGHT			8

const int led_gpios[] = {
	LED_BOTTOM_LEFT,
	LED_BOTTOM_MIDDLE,
	LED_BOTTOM_RIGHT,
	LED_EYE_LEFT,
	LED_BOTTOM_RIGHT
};
#define NUM_LEDS ARRAY_SIZE(led_gpios)

const int button_gpios[] = {
	BTN_L_UP,
	BTN_L_DOWN,
	BTN_L_LEFT,
	BTN_L_RIGHT,
	BTN_R_UP,
	BTN_R_DOWN,
	BTN_R_LEFT,
	BTN_R_RIGHT
};
#define NUM_BUTTONS ARRAY_SIZE(button_gpios)

#define DELAY 240000UL
#define DELAY2 60000UL

int num_flashes = 3;
int sleep_time = 15;
int led_on_time = 2000;

// Initialize with anything other than zero.
uint32_t lfsr_state = 1;
uint32_t old_buttons = ~0;

// pseudo random sequence
int lfsr()
{
    lfsr_state = ((((lfsr_state >> 31)  // Shift the 32nd bit to the first bit
        ^ (lfsr_state >> 6)             // XOR it with the seventh bit
        ^ (lfsr_state >> 4)             // XOR it with the fifth bit
        ^ (lfsr_state >> 2)             // XOR it with the third bit
        ^ (lfsr_state >> 1)             // XOR it with the second bit
        ^ lfsr_state)                   // and XOR it with the first bit.
        & 0x0000001)                    // Strip all the other bits off and
        <<31)                           // move it back to the 32nd bit.
        | (lfsr_state >> 1);            // Or with the register shifted right.
    return lfsr_state;
}

// Pulse eyes on and off.
void sequence1()
{
	qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);

    // Eyes up.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 69);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 69);
    }

    // Eyes down.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 69);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 69);
    }

    // Eyes up.
        for (int i = 0; i < 120; i++)
        {
            qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
            clk_sys_udelay(i * 69);
            qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
            clk_sys_udelay((120 - i) * 69);
        }

        // Eyes down.
        for (int i = 0; i < 120; i++)
        {
            qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
            clk_sys_udelay((120 - i) * 69);
            qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
            clk_sys_udelay(i * 69);
        }

    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
}

// Eye blinking left right
void sequence2()
{
	qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);

    for (int i = 0; i < 5; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
        clk_sys_udelay(20000);
        qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
        clk_sys_udelay(30000);
        qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
        clk_sys_udelay(20000);
        qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
        clk_sys_udelay(30000);
    }

    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
}

// Pulse eyes on and off, pulse all on and off
void sequence3()
{
	qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);

    // Eyes up.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 80);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 80);
    }

    // Eyes down.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 80);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 80);
    }

    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);

    // All up.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 80);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 80);
    }

    // All down.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 80);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 80);
    }

    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
}

// Slow spiral pulse
void sequence4()
{
	for (int a = 0; a < 5; a++)
	{
		for (int b = 0; b < (signed)NUM_LEDS; b++)
		{
			qm_gpio_set_pin(QM_GPIO_0, led_gpios[b]);

			// Up.
			for (int i = 0; i < 120; i++)
			{
				qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
				clk_sys_udelay(i * 20);
				qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
				clk_sys_udelay((120 - i) * 20);
			}

			// Down.
			for (int i = 0; i < 120; i++)
			{
				qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
				clk_sys_udelay((120 - i) * 20);
				qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
				clk_sys_udelay(i * 20);
			}

			qm_gpio_clear_pin(QM_GPIO_0, led_gpios[b]);
		}
	}
}

// Spiral loop left
void sequence5L(int speed)
{
	qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);

	qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(speed);

    qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
}

// Spiral loop right
void sequence5R(int speed)
{
	qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);

    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(speed);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(speed);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(speed);

    qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
}

// Pulse eyes twice, spiral twice, spiral loop
void sequence_start()
{
	qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);

    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(DELAY);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(DELAY);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(DELAY);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(DELAY);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(DELAY);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(DELAY);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(DELAY);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(DELAY);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(DELAY);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(DELAY);

    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(DELAY2);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    clk_sys_udelay(DELAY2);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(DELAY2);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    clk_sys_udelay(DELAY2);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(DELAY2);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    clk_sys_udelay(DELAY2);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(DELAY2);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    clk_sys_udelay(DELAY2);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(DELAY2);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    clk_sys_udelay(DELAY2);

    int delayTime = 50000;
    for (delayTime = 50000;
         delayTime >= 1000;
         delayTime -= delayTime / 10)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
        clk_sys_udelay(delayTime);
        qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
        clk_sys_udelay(delayTime);
        qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
        clk_sys_udelay(delayTime);
        qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
        clk_sys_udelay(delayTime);
        qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
        clk_sys_udelay(delayTime);
        qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
        clk_sys_udelay(delayTime);
        qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
        clk_sys_udelay(delayTime);
        qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
        clk_sys_udelay(delayTime);
        qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
        clk_sys_udelay(delayTime);
        qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    }

    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
    qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);

    // All up.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 69);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 69);
    }

    // All down.
    for (int i = 0; i < 120; i++)
    {
        qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay((120 - i) * 69);
        qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
        clk_sys_udelay(i * 69);
    }

    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
    qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
    qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);
}

bool poll_buttons()
{
	uint32_t cur_buttons = ~QM_GPIO[0]->gpio_ext_porta;
	uint32_t new_buttons = cur_buttons & ~old_buttons;
	old_buttons = cur_buttons;

	if (cur_buttons & BIT(BTN_R_UP))
	{

	}
	else if (new_buttons & BIT(BTN_L_RIGHT))
	{

	}
	else if (new_buttons & BIT(BTN_L_DOWN))
	{

	}
	else if (new_buttons & BIT(BTN_L_LEFT))
	{

	}
	else if (new_buttons & BIT(BTN_R_RIGHT))
	{

	}

	return new_buttons != 0;
}

void fast() {
	clk_sys_set_mode(CLK_SYS_HYB_OSC_4MHZ, CLK_SYS_DIV_1);
}
void slow() {
	clk_sys_set_mode(CLK_SYS_HYB_OSC_4MHZ, CLK_SYS_DIV_4);
}

int main()
{
	bool loadComplete = false;
	fast();

	qm_gpio_port_config_t cfg = {
		.direction = BIT(LED_BOTTOM_LEFT) | 
			BIT(LED_BOTTOM_MIDDLE) | 
			BIT(LED_BOTTOM_RIGHT) |
			BIT(LED_EYE_LEFT) | 
			BIT(LED_EYE_RIGHT) | 
			BIT(LED_ENABLE)
	};

	qm_gpio_set_config(QM_GPIO_0, &cfg);

	for (int i = 0; i < (signed)NUM_BUTTONS; i++)
	{
		qm_pmux_pullup_en(button_gpios[i], true);
		qm_pmux_input_en(button_gpios[i], true);
	}

	sequence1();
	clk_sys_udelay(1000000);

	sequence_start();
	clk_sys_udelay(DELAY);
	loadComplete = true;

	while(loadComplete)
	{
		qm_gpio_set_pin(QM_GPIO_0, LED_ENABLE);

		int delayTime = 50000;

		for (delayTime = 50000; delayTime >= 1000; delayTime -= delayTime / 1)
		{
			qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
			clk_sys_udelay(delayTime);
			qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_LEFT);
			clk_sys_udelay(delayTime);
			qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
			clk_sys_udelay(delayTime);
			qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_MIDDLE);
			clk_sys_udelay(delayTime);
			qm_gpio_set_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
			clk_sys_udelay(delayTime);
			qm_gpio_clear_pin(QM_GPIO_0, LED_BOTTOM_RIGHT);
			clk_sys_udelay(delayTime);
			qm_gpio_set_pin(QM_GPIO_0, LED_EYE_RIGHT);
			clk_sys_udelay(delayTime);
			qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_RIGHT);
			clk_sys_udelay(delayTime);
			qm_gpio_set_pin(QM_GPIO_0, LED_EYE_LEFT);
			clk_sys_udelay(delayTime);
			qm_gpio_clear_pin(QM_GPIO_0, LED_EYE_LEFT);
			clk_sys_udelay(delayTime);
		}

		qm_gpio_clear_pin(QM_GPIO_0, LED_ENABLE);

		for (int i = 0; i < sleep_time; i++)
		{
			if (poll_buttons())
			{
				break;
			}
			slow();
			clk_sys_udelay(100000);
			fast();
		}
	}
}
