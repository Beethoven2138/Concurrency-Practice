#include <mutex>
#include <thread>
#include <iostream>
#include <atomic>
#include <condition_variable>

class FizzBuzz {
private:
    int n;
    std::mutex mtx;
    std::condition_variable cond;
    int i = 1;
public:
    FizzBuzz(int n) {
        this->n = n;
    }

    // printFizz() outputs "fizz".
    void fizz() {
        while (i <= n){
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock, [&]{return (i>n||(i%3==0 && i%5!=0));});
            if (i <=n && i%3==0 && i%5!=0){
                std::cout << "Fizz" <<std::endl;
                ++i;
            }
            cond.notify_all();
        }
    }

    // printBuzz() outputs "buzz".
    void buzz() {
        while (i <= n){
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock, [&]{return (i>n||(i%5==0 && i%3!=0));});
            if (i <=n && i%5==0 && i%3!=0){
                std::cout << "Buzz" <<std::endl;
                ++i;
            }
            cond.notify_all();
        }
    }

  // printFizzBuzz() outputs "fizzbuzz".
	void fizzbuzz() {
        while (i <= n){
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock, [&]{return (i>n||(i%5==0 && i%3==0));});
            if (i <=n && i%5==0 && i%3==0){
                std::cout << "FizzBuzz"<<std::endl;
                ++i;
            }
            cond.notify_all();
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number() {
        while (i <= n){
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock, [&]{return (i>n||(i%5!=0 && i%3!=0));});
            if (i <=n && i%5!=0 && i%3!=0){
                std::cout << i <<std::endl;
                ++i;
            }
            cond.notify_all();
        }
    }
};

int main(){
    FizzBuzz test(150);
    std::jthread first([&]{test.buzz();});
    std::jthread second([&]{test.fizz();});
    std::jthread third([&]{test.fizzbuzz();});
    std::jthread fourth([&]{test.number();});
    return 0;
}
