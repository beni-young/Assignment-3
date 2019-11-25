#include <mutex>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>
#include <fstream>

std::ofstream tstFile;
std::mutex fout_mutex;

struct fork {
    std::mutex m;
};

struct philosopher {
  std::string name;
  int counter = 0;
  size_t left;
  size_t right;
  std::chrono::milliseconds Dining = std::chrono::milliseconds(0);
  std::chrono::milliseconds hungryTime = std::chrono::milliseconds(0);
  std::chrono::milliseconds eatTime = std::chrono::milliseconds(0);
  std::chrono::milliseconds forkTime1, forkTime2, forkTime3 = std::chrono::milliseconds(0);
  std::chrono::milliseconds thinkTime = std::chrono::milliseconds(0);
  std::mt19937 rng{std::random_device {}()};
  //ofstream tstFile;
  
    philosopher(const std::string &name_, size_t left_, size_t right_)
        : name(name_), left(left_), right(right_){}

  void eat(std::vector<fork> &table_)
  { 
    
    tstFile.open("negativeOne.txt", std::ios::out);
    thread_local std::uniform_int_distribution<int> rng_eating(10,40);
    thread_local std::uniform_int_distribution<int> rng_fork(50,100);
    auto start = std::chrono::high_resolution_clock::now();

    while(true) {
      
      if (table_[left].m.try_lock()){      
      std::lock_guard<std::mutex> l(table_[left].m, std::adopt_lock);
      auto stopwatch = std::chrono::high_resolution_clock::now();      
      hungryTime += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
      {
      std::lock_guard<std::mutex> cout_lock(fout_mutex);
      tstFile << name << ", Time: " << hungryTime.count() << " ms, " << " entering hungry state.\n";	
      }
      stopwatch = std::chrono::high_resolution_clock::now();      
      forkTime1 += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
      {
      std::lock_guard<std::mutex> cout_lock(fout_mutex);
      tstFile << name << ", Time: " << forkTime1.count() << " ms, " << " picked up left fork " 
      << left << "\n";
      }   
      }
      else {
        auto stopwatch = std::chrono::high_resolution_clock::now();      
        forkTime1 += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);

      {
        std::lock_guard<std::mutex> cout_lock(fout_mutex);
        tstFile  << name << ", Time: " << forkTime1.count() << " ms, "
        << " tried to pick up left fork " << left << ", its unavailable\n";
      }      
      std::this_thread::sleep_for(std::chrono::milliseconds(rng_fork(rng)));          
      continue;
      }

      if(table_[right].m.try_lock()) {
      auto stopwatch = std::chrono::high_resolution_clock::now();
      forkTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
      {
      std::lock_guard<std::mutex> cout_lock(fout_mutex);
      tstFile  << name << ", Time: " << forkTime2.count() << " ms, " << " picked up right fork " 
      << right << "\n";
      }      
      }
      else {
        auto stopwatch = std::chrono::high_resolution_clock::now();      
        forkTime3 += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
        {
        std::lock_guard<std::mutex> cout_lock(fout_mutex);
        tstFile  << name << ", Time: " << forkTime3.count() << " ms, "
        << " tried to pick up right fork " << right << ", its unavailable\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rng_fork(rng)));  
        continue;
      }

      auto stopwatch = std::chrono::high_resolution_clock::now();
      eatTime += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
      {
      std::lock_guard<std::mutex> cout_lock(fout_mutex);
      tstFile  << name << ", Time: " << eatTime.count() << " ms, " << " entering eating state. ";
      auto e_time = rng_eating(rng);
      tstFile  << "Will eat for " << e_time << " ms.\n";      
      std::this_thread::sleep_for(std::chrono::milliseconds(e_time));
      }

      table_[right].m.unlock();
      
      stopwatch = std::chrono::high_resolution_clock::now();
      thinkTime += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch - start);
      {
      std::lock_guard<std::mutex> cout_lock(fout_mutex);
      tstFile  << name << ", Time: " << thinkTime.count() << " ms, " << " entering thinking state\n";
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      auto finish = std::chrono::high_resolution_clock::now();
      auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

      tstFile  << "\n\n";
    {
      std::lock_guard<std::mutex> fout_lock(fout_mutex);
      Dining += milliseconds;
      counter++;
      
    }
      if (Dining >= std::chrono::milliseconds(60000)){
        //std::lock_guard<std::mutex> fout_lock(fout_mutex);
        
	      break;
      }
    }
  }
};

int main()
{
    std::vector<fork> table(4);
    std::vector<philosopher> philosophers;
    philosophers.emplace_back("Philosopher: 0", 0, 1);
    philosophers.emplace_back("Philosopher: 1", 1, 2);
    philosophers.emplace_back("Philosopher: 2", 2, 3);
    philosophers.emplace_back("Philosopher: 3", 3, 0);

    std::vector<std::thread> handles(4);


    for (auto i = 0; i < 4; ++i) {
        handles[i] =
            std::thread(&philosopher::eat, &philosophers[i], std::ref(table));
    }

    std::for_each(handles.begin(), handles.end(),
                  std::mem_fn(&std::thread::join));


  // Total and Average Hunger State for each Philosopher
  for (auto j = 0; j < 4; ++j){
  tstFile << philosophers[j].name << " Total Hunger State: " << philosophers[j].hungryTime.count() << "\t";

  tstFile << philosophers[j].name << " Average Hunger State: " 
  << philosophers[j].hungryTime.count() / 60000 << "\n";
  } 
  
  tstFile << "\n\n";
  
  // Total and Average Eating State for each Philosopher
  for (auto j = 0; j < 4; ++j){
  tstFile << philosophers[j].name << " Total Eating State: " << philosophers[j].eatTime.count() << "\t";

   tstFile << philosophers[j].name << " Average Eating State: " 
  << philosophers[j].eatTime.count() / 60000 << "\n";
  }
 
 tstFile << "\n\n";
 
  // Total and Average Thinking State for each Philosopher
  for (auto j = 0; j < 4; ++j){
  tstFile << philosophers[j].name << " Total Thinking State: " << philosophers[j].thinkTime.count() << "\t";

   tstFile << philosophers[j].name << " Average Thinking State: " 
  << philosophers[j].thinkTime.count() / 60000 << "\n";
  } 
  
  
  tstFile.close();

  return 0;
}
