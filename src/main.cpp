#include <Arduino.h>
#include <FastLED.h>

#include "configs.h"

volatile uint8_t interruptTriggered = false;

CRGB leds[RGB_LEDS_NUMBER];

void buttonPressed(void);
void setColor(const uint32_t color);
void setCHSVColor(const CHSV color);
void rainbow(const uint16_t delay_ms);

void setup()
{
	Serial.begin(BAUDRATE);
	pinMode(LED_FLASH_PIN, OUTPUT);

	FastLED.addLeds<WS2812, LED_STRIP_DATA_PIN, RGB>(leds, RGB_LEDS_NUMBER);

	pinMode(BRIGHTNESS_BUTTON_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BRIGHTNESS_BUTTON_PIN), buttonPressed, FALLING);
}

void loop()
{
	static uint32_t ledOffTime = 0;

	static uint16_t updateReccurence = 0;
	static uint32_t nextUpdateTime = 0;

	static uint8_t outputLevel = 0;
	static uint8_t previousOutputLevel = 0;
	static uint8_t deltaOutputLevel = 0;

	static uint32_t watchdogTimer = millis() + WATCH_DOG_TIMEOUT;

	static uint8_t greenToRedHue = INITIAL_HUE_VALUE;
	static uint8_t brightness = INITIAL_BRIGHTNESS;

	// Read new input level
	if (Serial.available())
	{
		int inputLevel = Serial.parseInt();
		Serial.read();

		previousOutputLevel = outputLevel;

		inputLevel = constrain(inputLevel, MIN_INPUT_LEVEL, MAX_INPUT_LEVEL);
		outputLevel = map(inputLevel, MIN_INPUT_LEVEL, MAX_INPUT_LEVEL, MIN_PWM_LEVEL, MAX_PWM_LEVEL);

		deltaOutputLevel = abs(outputLevel - previousOutputLevel);
		updateReccurence = int(GET_NEW_DATA_INTERVAL / deltaOutputLevel);
		
		digitalWrite(LED_FLASH_PIN, HIGH);
		ledOffTime = millis() + LED_FLASH_INTERVAL;
		watchdogTimer = millis() + WATCH_DOG_TIMEOUT;
	}
	
	// Smooth output level change
	if (deltaOutputLevel && (millis() >= nextUpdateTime))
	{
		nextUpdateTime = millis() + updateReccurence;

		if (outputLevel > previousOutputLevel)
		{
			previousOutputLevel++;
		}
		else if (outputLevel < previousOutputLevel)
		{
			previousOutputLevel--;
		}
		
		if (previousOutputLevel == outputLevel)
		{
			deltaOutputLevel = 0;
		}

		analogWrite(ANALOG_OUTPUT_PIN, previousOutputLevel);

        greenToRedHue = map(previousOutputLevel, 0, MAX_PWM_LEVEL, 0, MAX_HUE_VALUE);
		setCHSVColor(CHSV(greenToRedHue, SATURATION_VALUE, brightness));
	}

	// Turn off LED after flash interval
	if (millis() >= ledOffTime)
	{
		digitalWrite(LED_FLASH_PIN, LOW);
	}

	// On disconnect set output to 0 after timeout
	if (millis() >= watchdogTimer)
	{
		updateReccurence = int(GET_NEW_DATA_INTERVAL / previousOutputLevel);

		for (; previousOutputLevel; previousOutputLevel--)
		{
			analogWrite(ANALOG_OUTPUT_PIN, previousOutputLevel);
			delay(updateReccurence);
		}

		outputLevel = 0;
		previousOutputLevel = 0;
		deltaOutputLevel = 0;

		analogWrite(ANALOG_OUTPUT_PIN, previousOutputLevel);
		watchdogTimer = millis() + WATCH_DOG_TIMEOUT;
		setColor(CRGB::Black);
	}

	// Change brightness on button press
	if (interruptTriggered)
	{
		while (digitalRead(BRIGHTNESS_BUTTON_PIN) == LOW)
		{
			if (brightness == MAX_BRIGHTNESS)
			{
				brightness = MIN_BRIGHTNESS;
			}
			else
			{
				brightness++;
			}

			setCHSVColor(CHSV(greenToRedHue, SATURATION_VALUE, brightness));
			analogWrite(ANALOG_OUTPUT_PIN, brightness);

			if (brightness % BRIGHTNESS_STEP == 0)
			{
				delay(CHANGE_BRIGHTNESS_DELAY);
			}

			delay(CHANGE_BRIGHTNESS_SMOOTH_DELAY);
		}

		analogWrite(ANALOG_OUTPUT_PIN, previousOutputLevel);
		interruptTriggered = false;
	}
}

void setColor(const uint32_t color)
{
	for (int i = 0; i < RGB_LEDS_NUMBER; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}

void setCHSVColor(const CHSV color)
{
	for (int i = 0; i < RGB_LEDS_NUMBER; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}

void rainbow(const uint16_t delay_ms = 100)
{
	for (int i = 0; i < 255; i++)
	{
		leds[0].setHue(i);
		leds[1].setHue((i + 8) % 255);
		leds[2].setHue((i + 16) % 255);
		leds[3].setHue((i + 24) % 255);
		FastLED.show();
		delay(delay_ms);
	}
}

void buttonPressed(void)
{
	interruptTriggered = true;
}
