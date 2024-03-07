#include "Subject.h"

void Subject::addObserver(Observer *observer)
{
    mObservers.push_back(observer);
}