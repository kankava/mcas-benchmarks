#include <iostream>

#include <atomic>
#include <thread>

#include "Deque.h"

lockfree_mcas::Deque deque;

std::atomic<int> data;
void do_work()
{
    int val = data.fetch_add(1, std::memory_order_relaxed);
    deque.push_back(val);
}

void do_work2()
{
     deque.pop_front();
}

int main()
{
    /*
    std::thread th1(do_work);
    std::thread th2(do_work);
    std::thread th3(do_work);
    std::thread th4(do_work2);
    std::thread th5(do_work2);
 
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
*/
    deque.push_back(1);
    deque.push_back(2);
    deque.push_back(3);
    deque.push_back(4);
    deque.print_all();

    deque.pop_front();
    deque.pop_front();
    deque.print_all();
 
    std::cout << "Result:" << data << '\n';
}
