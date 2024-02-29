#include "RotSW.h"

//
void RotSW::InterruptHandler(uint gpio, uint32_t event_mask)
{
    RotSW_event event;
    if (gpio == mRotAPin) {
        switch (event_mask) {
            case GPIO_IRQ_EDGE_RISE:
                event = gpio_get(mRotBPin) ? CLOCKWISE : COUNTER_CLOCKWISE;
                break;
            case GPIO_IRQ_EDGE_FALL:
                event = gpio_get(mRotBPin) ? COUNTER_CLOCKWISE : CLOCKWISE;
                break;
            default:
                event = NOTHING;
        }
    } else {
        event = PRESS;
    }
    mEvents.push(event);
}

RotSW::RotSW(uint press_pin, uint rot_A_pin, uint rot_B_pin)
{
    mPressPin = press_pin;
    mRotAPin = rot_A_pin;
    mRotBPin = rot_B_pin;

    gpio_pull_up(mPressPin);
    gpio_init(mPressPin);
    gpio_init(mRotBPin);

    gpio_set_irq_enabled_with_callback(mPressPin,
                                       GPIO_IRQ_EDGE_FALL,
                                       true,
                                       InterruptHandler);
    gpio_set_irq_enabled_with_callback(mRotAPin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true,
                                       InterruptHandler);
}


RotSW::operator RotSW_event()
{
    RotSW_event event;
    if (!mEvents.empty()) {
        event = mEvents.front();
        mEvents.pop();
    } else {
        event = NOTHING;
    }
    return event;
}