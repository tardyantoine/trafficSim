#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
TrafficLightPhase MessageQueue::receive()
{
    std::unique_lock<std::mutex> uLock(_qMutex);
    _qCondition.wait(uLock, [this] {return !_queue.empty(); });

    auto msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

void MessageQueue::send(TrafficLightPhase &&msg)
{
    std::lock_guard<std::mutex> uLock(_qMutex);
    _queue.push_back(std::move(msg));
    _qCondition.notify_one();
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(1) {
        if(_mQueue.receive() == TrafficLightPhase::green) { return; }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));   
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    int cycleDuration = rand()%(2001) + 4000; // Rand ms between 4000 and 6000
    auto lastUpdate = std::chrono::system_clock::now();

    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid overloading CPU
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        if(timeSinceLastUpdate >= cycleDuration) {
            lastUpdate = std::chrono::system_clock::now();
            
            // Toggle phase
            _currentPhase == TrafficLightPhase::red ? _currentPhase = TrafficLightPhase::green : _currentPhase = TrafficLightPhase::red;
            auto phase = _currentPhase;
            _mQueue.send(std::move(phase));
        }
    }
}