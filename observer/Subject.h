#ifndef SUBJECT_H
#define SUBJECT_H

#include "Observer.h"

#include <memory>

// Subject class for observer pattern.
class Subject
{
  public:
    virtual ~Subject() = default;
    virtual void addObserver(std::shared_ptr<Observer> observer) = 0;
    virtual void notifyObservers() = 0;
};

#endif /* SUBJECT_H */
