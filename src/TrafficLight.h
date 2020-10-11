#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase {
    red,
    green 
};

class MessageQueue
{
public:
    TrafficLightPhase receive();
    void send(TrafficLightPhase &&msg);

private:
    std::deque<TrafficLightPhase> _queue;
    std::mutex _qMutex;
    std::condition_variable _qCondition;
}; 

class TrafficLight : TrafficObject
{
public:
    TrafficLight();
    ~TrafficLight() {}
    TrafficLightPhase getCurrentPhase();

    void waitForGreen();
    void simulate();

private:
    void cycleThroughPhases();

    MessageQueue _mQueue;
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
};

#endif