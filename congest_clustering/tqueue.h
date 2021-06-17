#ifndef INCLUDED_QUEUE_H
#define INCLUDED_QUEUE_H


#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
template<class T>
class BoundedQueue 
{
	public:
		BoundedQueue(int max) 
			: m_begin(0)
			  , m_end(0)
			  , m_buffered(0)
			  , m_circularBuf(max) 
	{
	}
	BoundedQueue(const BoundedQueue & rhs)=delete;
	BoundedQueue& operator=(const BoundedQueue & rhs)=delete;
	void put(const T &m) 
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		m_notFull.wait(lk,[this]{return m_buffered<m_circularBuf.size();});

		purePut(m);
		lk.unlock();
		m_notEmpty.notify_one();
	}

	void putHead(const T& m)
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		m_notFull.wait(lk,[this]{return m_buffered<m_circularBuf.size();});

		purePutHead(m);
		lk.unlock();
		m_notEmpty.notify_one();
	}

	bool tryPut(const T &m)
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		if(m_buffered >= m_circularBuf.size())
		{
			return false;
		}
		else
		{
			purePut(m);
			lk.unlock();
			m_notEmpty.notify_one();
			return true;
		}
	}

	bool tryPutHead(const T &m)
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		if(m_buffered >= m_circularBuf.size())
		{
			return false;
		}
		else
		{
			purePutHead(m);
			lk.unlock();
			m_notEmpty.notify_one();
			return true;
		}
	}

	void get(T &m) 
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		m_notEmpty.wait(lk,[this]{return m_buffered>0;});
		pureGet(m);
		lk.unlock();
		m_notFull.notify_all();
	}

	bool get(T &m, int wait)
	{
		bool signaled = false;
		std::unique_lock<std::mutex> lk(m_monitor);
		signaled = m_notEmpty.wait_for(lk,std::chrono::seconds(wait),[this]{return m_buffered>0;});
		if(!signaled)
		  return false;

		pureGet(m);
		lk.unlock();
		m_notFull.notify_all();
		return true;
	}

	bool tryGet(T &m)
	{
		std::unique_lock<std::mutex> lk(m_monitor);
		if(m_buffered <= 0)
		{
			return false;
		}
		else
		{
			pureGet(m);
			lk.unlock();
			m_notFull.notify_all();
			return true;
		}
	}

	size_t size() 
	{
		return m_buffered;
	}
private:
	inline void purePut(const T& m)
	{
		m_circularBuf[m_end] = m;
		m_end = (m_end+1) % m_circularBuf.size();
		++m_buffered;
	}
	inline void purePutHead(const T& m)
	{
		if(m_begin == 0)
		  m_begin = m_circularBuf.size();
		m_begin--;
		m_circularBuf[m_begin] = m;
		++m_buffered;
	}
	inline void pureGet(T& m)
	{
		m = m_circularBuf[m_begin];
		// clear the memory
		T t;
		std::swap(m_circularBuf[m_begin], t);
		m_begin = (m_begin+1) % m_circularBuf.size();
		--m_buffered;
	}
private:
	size_t m_begin;
	size_t m_end;
	size_t m_buffered;
	std::vector<T> m_circularBuf;
	std::condition_variable m_notFull;
	std::condition_variable m_notEmpty;
	std::mutex m_monitor;
};
#endif // INCLUDED_QUEUE_H

