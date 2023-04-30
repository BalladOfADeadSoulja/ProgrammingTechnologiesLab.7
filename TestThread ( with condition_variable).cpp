#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

const int N = 10; // количество пчел
const int H = 5; // вместимость горшка

mutex mtx; // мьютекс для синхронизации доступа к горшку
condition_variable cv_bear, cv_bees; // условные переменные для уведомления медведя и пчел

int honey_pot = 0; // количество меда в горшке

void bear()
{
    unique_lock<mutex> lock(mtx);
    while (true) {
        cv_bear.wait(lock, [] { return honey_pot == H; }); // ждем, пока горшок не наполнится
        cout << "Bear is awake and eats all the honey!" << endl;
        honey_pot = 0; // опустошаем горшок
        cv_bees.notify_all(); // уведомляем пчел
        cout << "Bear is sleeping..." << endl;
    }
}

void bee(int id)
{
    while (true) {
        this_thread::sleep_for(chrono::seconds(1)); // имитируем время, необходимое для сбора меда
        unique_lock<mutex> lock(mtx);
        if (honey_pot < H) {
            honey_pot++; // кладем глоток меда в горшок
            cout << "Bee " << id << " brings a drop of honey. Honey pot: " << honey_pot << endl;
            if (honey_pot == H) {
                cv_bear.notify_one(); // будим медведя
            }
        }
        else {
            cv_bees.wait(lock); // ждем, пока медведь не опустошит горшок
        }
    }
}

int main()
{
    thread t_bear(bear);
    thread t_bees[N];
    for (int i = 0; i < N; i++) {
        t_bees[i] = thread(bee, i);
    }
    t_bear.join();
    for (int i = 0; i < N; i++) {
        t_bees[i].join();
    }
    return 0;
}