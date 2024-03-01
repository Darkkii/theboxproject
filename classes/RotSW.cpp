#include "RotSW.h"

void RotSW::interruptHandler(uint gpio, uint32_t event_mask)
{
    if (gpio == mRotAPin) {
        switch (event_mask) {
            case GPIO_IRQ_EDGE_RISE:
                mEvents.push(gpio_get(mRotBPin) ? CLOCKWISE : COUNTER_CLOCKWISE);
                break;
            case GPIO_IRQ_EDGE_FALL:
                mEvents.push(gpio_get(mRotBPin) ? COUNTER_CLOCKWISE : CLOCKWISE);
        }
    } else {
        switch (event_mask) {
            case GPIO_IRQ_EDGE_RISE:
                mPrevRise = time_us_64();
                break;
            case GPIO_IRQ_EDGE_FALL:
                if (time_us_64() - mPrevRise > PRESS_DEBOUNCE_DELAY_US) {
                    mEvents.push(PRESS);
                }
        }
    }
}

// Initialize static members
uint RotSW::mPressPin;
uint64_t RotSW::mPrevRise = 0;
uint RotSW::mRotAPin;
uint RotSW::mRotBPin;
std::queue<RotSW_event> RotSW::mEvents;

RotSW::RotSW(uint press_pin, uint rot_A_pin, uint rot_B_pin)
{
    mPressPin = press_pin;
    mRotAPin = rot_A_pin;
    mRotBPin = rot_B_pin;

    gpio_pull_up(mPressPin);
    gpio_init(mPressPin);
    gpio_init(mRotBPin);

    gpio_set_irq_enabled_with_callback(mPressPin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true,
                                       interruptHandler);
    gpio_set_irq_enabled_with_callback(mRotAPin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true,
                                       interruptHandler);
}


RotSW_event RotSW::getEvent()
{
    RotSW_event event;
    if (!mEvents.empty()) {
        event = mEvents.front();
        mEvents.pop();
    } else {
        event = NO_EVENT;
    }
    return event;
}