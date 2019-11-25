#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <condition_variable>
#include <functional>
#include <algorithm>


namespace {
    int sleep_for_random_time() {
      thread_local std::mt19937 mt{ std::random_device{}() };
        //std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(10, 40);
//std::this_thread::sleep_for(std::chrono::milliseconds(rng_fork(rng)));
        return dist(mt);
    }
}

class DiningPhilosopher
{

public:
  
  DiningPhilosopher(int numOfPhilosophers_ = 4, int numOfForks_ = 4, int numOfEating_ = 3)
    : numOfPhilosophers(numOfPhilosophers_),
      numOfForks(numOfForks_),
      numOfEating(numOfEating_)
  {
 
    forks.push_back(new Fork);
    forks.push_back(new Fork);
    forks.push_back(new Fork);
    forks.push_back(new Fork);

    numAllowed = numOfForks - 1;
  }

  ~DiningPhilosopher() {
     for(const auto& fork : forks) {
      delete fork;
     }
  }

  DiningPhilosopher(const DiningPhilosopher&) = delete;
  DiningPhilosopher& operator = (const DiningPhilosopher&) = delete;

  void StartDining()
  {

    for(int i = 0; i < numOfPhilosophers; ++i)
      {
	threads.push_back(std::thread(&DiningPhilosopher::Philosopher, this, i));
      }
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

  }
  
private:
  
    struct Fork {
    std::mutex mux;
  };
  

  int numOfEating;
  int numOfPhilosophers;
  int numAllowed;
  int numOfForks;
  std::vector<std::thread> threads;
  std::vector<Fork*> forks;
  std::mutex permission;
  std::condition_variable_any cv;

  void WaitForPermission()
  {
    std::lock_guard<std::mutex> lock(permission);
    cv.wait(permission, [this] { return numAllowed > 0; });
    --numAllowed;
  }

  void GrantPermission() {
    std::lock_guard<std::mutex> lock(permission);
    ++numAllowed;
    
    if(numAllowed == 1) {
      cv.notify_all();
    }
  }
  

  void Eating(int id) {
    std::chrono::milliseconds duration(sleep_for_random_time());
    std::cout << "Will eat for " << duration.count() << " ms.\n";
    std::this_thread::sleep_for(duration);
  }
  

  void Think(int id) {
    std::chrono::milliseconds duration(10);
    std::this_thread::sleep_for(duration);
  }
  
  void Eat(int id) {
    this->WaitForPermission();
    forks[id]->mux.lock();
    forks[(id + 1) % numOfForks]->mux.lock();
    std::cout << "Philosopher: " << "picked up fork " << forks[id] 
    std::cout << "Philosopher: " << id << " entering eating state.";
    this->Eating(id);
    std::cout << "Philosopher "<< id << " entering hungry state" << std::endl;
    
    forks[id]->mux.unlock();
    forks[(id + 1) % numOfForks]->mux.unlock();
    
    this->GrantPermission();
  }
  
  void Philosopher(int id) {
    for(int i = 0; i < numOfEating; ++i) {
      this->Think(id);
      this->Eat(id);
    }
    
    std::cout << "Philosopher P" << id << " DONE" << std::endl;
  }


 
};


int main()
{
  DiningPhilosopher dp;
  dp.StartDining();
  
  return 0;
}
