#ifndef _BUSINESS_HPP_
#define _BUSINESS_HPP_
#include <memory>
#include <map>
#include <list>
#include <vector>
class provincesOfBusiness;
class congestObj;
struct business{
	virtual void workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){}
	virtual void on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co)=0;
	virtual void on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co)=0;
	virtual void on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co)=0;
	virtual ~business(){}
};

struct mapVersionBus:business{
	virtual void workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
};
struct tagServiceBus:business{
	virtual void workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
};
struct ttiJobBus:business{
	virtual void workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
	virtual void on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co);
};
struct businessManager{
	static businessManager* inst();
	void getBusiness(uint32_t type,std::list<business*> &blist);
private:
	businessManager();
	businessManager(const businessManager&rhs)=delete;
	businessManager & operator=(const businessManager&rhs)=delete;
	std::vector<std::unique_ptr<business>> m_allBus;
};
#endif
