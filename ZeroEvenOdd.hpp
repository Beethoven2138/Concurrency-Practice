#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

class ZeroEvenOdd {
private:
    int n;
    bool print_odd = false;
    bool print_even = false;
    std::atomic<bool> finished = false;
    std::mutex odd_mtx;
    std::condition_variable odd_done;
    std::mutex even_mtx;
    std::condition_variable even_done;

public:
    ZeroEvenOdd(int n) {
        this->n = n;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        std::unique_lock<std::mutex> odd_lock(odd_mtx);
        std::unique_lock<std::mutex> even_lock(even_mtx);
        while (!finished){
            printNumber(0);
            print_odd = true;
            odd_done.notify_one();
            odd_done.wait(odd_lock, [&]{return !print_odd;});
            print_even = true;
            if (!finished)
                printNumber(0);
            even_done.notify_one();
            if (finished)
                break;
            even_done.wait(even_lock, [&]{return !print_even;});
        }
    }

    void even(function<void(int)> printNumber) {
        std::unique_lock<std::mutex> even_lock(even_mtx);
        for (int i = 2; i <= n; i += 2){
            even_done.wait(even_lock, [&]{return print_even;});
            printNumber(i);
            print_even = false;
            if (i == n)
                finished = true;
            even_done.notify_one();
        }
    }

    void odd(function<void(int)> printNumber) {
        std::unique_lock<std::mutex> odd_lock(odd_mtx);
        for (int i = 1; i <= n; i += 2){
            odd_done.wait(odd_lock, [&]{return print_odd;});
            printNumber(i);
            print_odd = false;
            if (i == n)
                finished = true;
            odd_done.notify_one();
        }
    }
};
