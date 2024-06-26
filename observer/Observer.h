#ifndef OBSERVER_H
#define OBSERVER_H

class SettingsMessage;

// Observer class for observer pattern.
class Observer
{
  public:
    virtual ~Observer() = default;
    virtual void update() = 0;
    virtual void update(SettingsMessage sm) = 0;
};

#endif /* OBSERVER_H */
