#ifndef SUBJECT_H
#define SUBJECT_H

#include <memory>
#include "Observer.h"

class Subject
{
public:
    virtual void addObserver(std::shared_ptr<Observer> observer) = 0;
    virtual void notifyObservers() = 0;
};

#endif /* SUBJECT_H */
