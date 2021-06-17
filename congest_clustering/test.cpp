#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable> 
#include <atomic>
int i=0;
std::mutex m_monitor;
std::condition_variable cv1;
std::condition_variable cv2;
bool flag=false;
bool running=true;
void p1(){
	do{
	std::unique_lock<std::mutex> lk(m_monitor);
	cv1.wait(lk,[](){return !flag;});
	i++;
	if(i>100){
		running=false;
	}else{
		std::cout <<"A:"<<i<<std::endl;	
	}
	flag=true;
	lk.unlock();
	cv2.notify_one();
	}while(running);
}
void p2(){
	do{
	std::unique_lock<std::mutex> lk(m_monitor);
	cv2.wait(lk,[]{return flag;});
	i++;
	if(i>100){
		running=false;
	}else{
		std::cout <<"B:"<<i<<std::endl;	
	}
	flag=false;
	lk.unlock();
	cv1.notify_one();
	}while(running);
}
int main(){
	std::thread t1(p1);
	std::thread t2(p2);
	t1.join();t2.join();
	return 0;
}
