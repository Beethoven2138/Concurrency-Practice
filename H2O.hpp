#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>


class H2O {
    char hydrogen_count = 0;
    bool oxygen_present = false;
    std::mutex mtx;
    std::condition_variable cond;
public:
    H2O() {
        
    }
    void hydrogen(function<void()> releaseHydrogen) {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [&]{return hydrogen_count < 2;});
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        releaseHydrogen();
        ++hydrogen_count;
        if (hydrogen_count == 2){
            if (oxygen_present){
                oxygen_present = false;
                hydrogen_count = 0;
            }
        }
        cond.notify_all();
    }

    void oxygen(function<void()> releaseOxygen) {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [&]{return !oxygen_present;});
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        oxygen_present = true;
        if (hydrogen_count == 2){
            oxygen_present = false;
            hydrogen_count = 0;
        }
        cond.notify_all();
    }
};
