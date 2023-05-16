#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

std::condition_variable potFullCondition;

class HoneyPot {
private:
    int honey = 0;
    const int maxHoney = 10;

public:
    void addHoney(int honey) {
        this->honey += honey;
        if (this->honey > maxHoney) {
            this->honey = maxHoney;
        }
    }

    int getHoney() {
        return honey;
    }

    bool isFull() {
        return honey == maxHoney;
    }
};

class Bee {
public:
    void collectHoney(HoneyPot& pot) {
        pot.addHoney(1);
        int a = rand()%20;
        std::cout << "Bee " << a << " give 1 honey\n";
    }

    void run(HoneyPot& pot, std::mutex& mutex) {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            if (pot.isFull()) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                collectHoney(pot);
                if (pot.isFull()) {
                    std::cout << "HoneyPot is full\n";
                    lock.unlock();
                    potFullCondition.notify_one();
                }
                else {
                    lock.unlock();
                }
            }
        }
    }
};

class Bear {
public:
    void eatHoney(HoneyPot& pot) {
        int honey = pot.getHoney();
        pot.addHoney(-honey);
    }

    void run(HoneyPot& pot, std::mutex& mutex) {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            potFullCondition.wait(lock, [&pot] { return pot.isFull(); });
            eatHoney(pot);
            std::cout << "Bear eat all honey\n\n";
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

int main() {
    HoneyPot pot;
    Bee bee;
    Bear bear;

    std::mutex potMutex;
    std::mutex bearMutex;

    for (int i = 0; i < 100; i++)
    {
        std::thread beeThread(&Bee::run, &bee, std::ref(pot), std::ref(potMutex));
        std::thread bearThread(&Bear::run, &bear, std::ref(pot), std::ref(bearMutex));

        beeThread.join();
        bearThread.join();
        
        if (std::getchar() != ' ') { }//на нажатие мыши программа останавливается  
    }
    
    return 0;
}
