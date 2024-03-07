#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include "Observer.h"

class Subject
{
private:
    std::vector<Observer *> mObservers;

public:
    explicit Subject() {}
    void addObserver(Observer *observer);
    virtual void notify() = 0;
};

#endif /* SUBJECT_H */
