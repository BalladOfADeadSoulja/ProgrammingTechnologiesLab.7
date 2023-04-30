#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int N = 10; // количество пчел
const int H = 5; // вместимость горшка

mutex mtx; // мьютекс для синхронизации доступа к горшку

int honey = 0; // количество меда в горшке

void bear()
{
    while (true) {
        mtx.lock();
        if (honey == H)
        {
            cout << "Bear eat all honey" << '\n';
            cout << endl;
            honey = 0;
        }
        mtx.unlock();
        if (honey == 0)
        {
            cout << "Bear sleep" << '\n';
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}

void bee(int id)
{
    while (true) {
        mtx.lock();
        if (honey < H)
        {
            honey++;
            cout << "Bee " << id << " drop honey. Honey: " << honey << '\n';
            if (honey == H)
            {
                cout << "Bee " << id << " woke up the bear" << '\n';
            }
        }
        mtx.unlock();
        this_thread::sleep_for(chrono::seconds(1));
        /*if (honey == H)
            break;*/
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