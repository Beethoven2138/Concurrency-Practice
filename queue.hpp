#pragma once

#include <mutex>
#include <thread>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <algorithm>
#include <stdexcept>

template <typename T, size_t size>
class Queue{
    std::unique_ptr<T[]> arr;
    size_t current_length;
    mutable std::mutex mtx;
    std::condition_variable cond_add;
    std::condition_variable cond_remove;
public:
    Queue(){
        arr = std::make_unique<T[]>(size);
        current_length = 0;
    }
    Queue(std::initializer_list<T> entries){
        arr = std::make_unique<T[]>(size);
        if (entries.size() > size)
            throw std::runtime_error("Added too many entries into the queue");
        std::copy(entries.begin(), entries.end(), arr.get());
        current_length = entries.size();
    }
    Queue(const Queue<T, size> &cpy){
        std::lock_guard<std::mutex> lock(cpy.mtx);
        arr = std::make_unique<T[]>(size);
        current_length = cpy.current_length;
        std::copy(cpy.arr.get(), cpy.arr.get()+size, arr.get());
    }
    Queue<T, size>& operator = (const Queue<T, size> &cpy){
        if (&cpy != this){
            std::scoped_lock lock(cpy.mtx, mtx);
            arr = std::make_unique<T[]>(size);
            current_length = cpy.current_length;
            std::copy(cpy.arr.get(), cpy.arr.get()+size, arr.get());
        }
        return *this;
    }
    Queue(Queue<T, size> &&mv) noexcept {
        std::scoped_lock lock(mv.mtx);
        arr = std::move(mv.arr);
        current_length = mv.current_length;
        mv.current_length = 0;
    }
    Queue<T, size>& operator = (Queue<T, size> &&mv) noexcept {
        if (&mv != this){
            std::scoped_lock lock(mv.mtx, mtx);
            arr = std::move(mv.arr);
            current_length = mv.current_length;
            mv.current_length = 0;
        }
        return *this;
    }
    void add_entry(const T& entry){
        std::unique_lock<std::mutex> lock(mtx);
        if (arr == nullptr)
            throw std::runtime_error("Attempting to access a moved object");
        cond_add.wait(lock, [this]{return current_length < size;});
        arr[current_length++] = entry;
        cond_remove.notify_one();
    }
    T remove_entry(){
        std::unique_lock<std::mutex> lock(mtx);
        if (arr == nullptr)
            throw std::runtime_error("Attempting to access a moved object");
        cond_remove.wait(lock, [this]{return current_length > 0;});
        T ret = arr[0];
        for (size_t i = 0; i < std::min(current_length, size-1); ++i)
            arr[i] = arr[i+1];
        --current_length;
        cond_add.notify_one();
        return ret;
    }
};
