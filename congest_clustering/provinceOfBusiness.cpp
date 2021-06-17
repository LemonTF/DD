#include "provinceOfBusiness.h"
#include "business.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <sstream>
#include "overallDataManagement.h"
#include "TrafficMeta.pb.h"
#include "md5.h"
#include "LogUtils.h"
#include "CommonTools.h"
#include "MapManager.h"
#include "congestObject.h"
#include "mydb.h"
std::string linksinfo::getLinkPointString()const{
	std::string s("(");
	for(const auto &p:points){
		s=s+std::to_string(p.point_x/100000.0)+" "+std::to_string(p.point_y/100000.0)+",";
	}
	s.pop_back();
	s+=")";
	return s;
}
provincesOfBusiness::provincesOfBusiness(uint32_t id,uint32_t type,const std::string &data)
	:m_id(id)
	,m_type(type)
	,m_bFlag(false)
	,m_mapUpdateFlag(false)
	,m_mapChangedFlag(true)
	,m_info(data)
{
	businessManager::inst()->getBusiness(type,m_blist);
	m_pThread.reset(new std::thread(&provincesOfBusiness::run,this));
}
#if 0
void provincesOfBusiness::doBusiness(){
	for(const auto &pb:m_blist){
		pb->doBusiness(shared_from_this());
	}
}
#endif
bool provincesOfBusiness::getTrafficStatusData(const time_t time_interval){
	bool flag=false;
	unsigned char* szSrcBuffer;
	unsigned char* szDstBuffer;
	uint32_t batch_time=0;
	do{
		std::string strURL = "http://100.69.238.51:8696/traffic-dlr/getData?vid=";
		char szUser_pwd[128] = { 0 };
		std::string strPassword = "TTI-Calc";
		sprintf(szUser_pwd, "%sDLR%ld", strPassword.c_str(),time_interval);

		MD5 md5;
		md5.reset();
		md5.update(szUser_pwd, strlen(szUser_pwd));
		std::string strMD5 = md5.toString();

		strURL= strURL + strMD5 + "&citycode=" + std::to_string(m_id)+"&ts="+std::to_string(time_interval)+"&username=Calc-TTI";

		std::string strResponse;
		CURLcode nRes = CommonTools::HttpGet(strURL, strResponse,300);
		size_t nSrcLength = strResponse.length();

		NOTICE("url:"+strURL);

		if (nSrcLength == 0){
			FATAL("Can't fetch realtime traffic data,url:"+strURL+",error_code:"<<nRes);
			return true;
		}
		szSrcBuffer = new unsigned char[nSrcLength];
		memcpy(szSrcBuffer,strResponse.c_str(),nSrcLength);

		uint32_t nDstLength = 1024 * 1024 * 60;
		szDstBuffer = new unsigned char[nDstLength];

		bool bRes = CommonTools::DecoderGz(szSrcBuffer,(uint32_t)nSrcLength,szDstBuffer,nDstLength);
		if (!bRes){
			FATAL("Can't Decode unzip Traffic Data");
			flag=true;break;
		}
		its::service::TrafficData trafficData;
		if (trafficData.ParseFromArray(szDstBuffer, nDstLength)){
			uint32_t btime=trafficData.meta().batch_time();
			if(btime==batch_time){
				FATAL("province_id:"<<m_id<<":Get Same batch time:"<<CommonTools::unixtime2str(btime));
				flag=true;break;
			}
			batch_time=btime;
			NOTICE("batch_time:"<<m_id<<",batch_time:"<<batch_time);
			std::string mv=trafficData.meta().extra_values().Get(0);
			if(m_mapVersion!=mv){
				m_mapVersion=mv;
				m_mapUpdateFlag=true;
			}
			std::vector<uint64_t> vlinks;
			std::map<uint64_t,linksinfo> maplinks;
			for(int i=0;i<trafficData.flow().length_arr_size();i++){
				if(trafficData.flow().status_arr(i)>=3){
					uint64_t linkid = trafficData.flow().link_arr(i);
#if 1
					if(!m_mapTool->ExistsLink(linkid))continue;
#endif
					int level=0;
					m_mapTool->GetLevelByLinkID(linkid,level);
					if(level>3)continue;
					uint32_t congest_level = trafficData.flow().status_arr(i);
					uint32_t length =trafficData.flow().length_arr(i);
					double	 speed = trafficData.flow().speed_without_light_arr(i)/10.0;
					if(fabs(speed)<1e-6) speed=0.001;
					uint32_t confidence =  trafficData.flow().confidence_arr(i);
					NOTICE("Congest_Linkid:"<<m_id<<":"<<linkid<<":"<<congest_level<<":"<<length<<":"<<speed<<":"<<confidence);
					vlinks.push_back(linkid);
					linksinfo ls(linkid,confidence,congest_level,level,speed);
					maplinks.insert({linkid,ls});
				}
			}
			///////
			//std::cout <<"Link_size:"<<vlinks.size()<<std::endl;
			std::sort(vlinks.begin(),vlinks.end());
			std::vector<uint64_t>	vs;
			std::set_intersection(vlinks.begin(),vlinks.end(),m_linksList.begin(),m_linksList.end(),std::back_inserter(vs));
			//exist
			for(const auto &id:vs){
				linksinfo x=maplinks[id];
				m_linksInfoList[id].set(x.confidence,x.congest_level,x.speed);
				//NOTICE(m_id<<"_Same:"<<id<<"---"<<m_linksInfoList[id].road_level<<"----"<<m_linksInfoList[id].length);
			}
			vs.clear();
			std::set_difference(vlinks.begin(),vlinks.end(),m_linksList.begin(),m_linksList.end(),std::back_inserter(vs));
			for(const auto &id:vs){
				linksinfo x=maplinks[id];
#if 1
				//std::cout <<"Link_size:"<<vs.size()<<"&&&"<<id<<std::endl;
				m_mapTool->GetPointsByLinkID(id,x.points);
				m_mapTool->GetLengthByLinkID(id,x.length);
#endif
				m_linksInfoList[id]=x;
				//NOTICE(m_id<<"_New:"<<id<<"---"<<x.road_level<<"----"<<x.length<<"---"<<x.points.size());
			}
			vs.clear();
			std::set_difference(m_linksList.begin(),m_linksList.end(),vlinks.begin(),vlinks.end(),std::back_inserter(vs));
			//for(const auto &id:vs){
				//NOTICE(m_id<<"_Delete:"<<id);
			//}
			std::for_each(vs.begin(),vs.end(),[this](uint64_t id){m_linksInfoList.erase(id);});
			m_linksList=vlinks;
			//NOTICE("Link_size_"<<m_id<<",size="<<maplinks.size()<<"======="<<m_linksInfoList.size());
		}
	}while(0);
	delete szSrcBuffer;
	delete szDstBuffer;
	return flag;
}

struct linkidLevel{
	linkidLevel(int len,uint64_t id){
		length=len;
		linkid=id;
	}
	int length=0;
	uint64_t linkid=0;
};

std::vector<uint64_t> provincesOfBusiness::subClustering(uint64_t rootlink,int type){
	linkidLevel ll(0,rootlink);
	std::queue<linkidLevel> linkInfo;
	linkInfo.push(ll);
	std::vector<uint64_t> alllinks;
	while(!linkInfo.empty()){
		linkidLevel sll=linkInfo.front();linkInfo.pop();
		//std::cout <<"circle2:"<<sll.linkid<<std::endl;
		if(sll.length<200){
			std::vector<uint64_t> lklist;
			type?m_mapTool->GetPreLinksByLinkID(sll.linkid,lklist):m_mapTool->GetNextLinksByLinkID(sll.linkid,lklist);
			std::vector<uint64_t> xv;
			std::sort(lklist.begin(),lklist.end());
			std::sort(alllinks.begin(),alllinks.end());
			std::set_difference(lklist.begin(),lklist.end(),alllinks.begin(),alllinks.end(),std::back_inserter(xv));
			std::copy(xv.begin(),xv.end(),std::back_inserter(alllinks));
			for(const auto & sid:xv){
				int slength=0;
				m_mapTool->GetLengthByLinkID(sid,slength);
				linkidLevel tmp(sll.length+slength,sid);
				linkInfo.push(tmp);
			}
		}
	}
	std::sort(alllinks.begin(),alllinks.end());
	auto last=std::unique(alllinks.begin(),alllinks.end());
	alllinks.erase(last,alllinks.end());
	return alllinks;
	
}

void provincesOfBusiness::makeCongestClustering(){
	std::list<std::vector<uint64_t>> clusteringList;
	std::vector<uint64_t> has_visited;
	int count =0;
	for(const auto &id:m_linksList){
		std::vector<uint64_t> clusterV;
		std::vector<uint64_t> alllinks;
		clusterV.clear();alllinks.clear();
		std::queue<uint64_t> link_queue;
		link_queue.push(id);

		while(!link_queue.empty()){
			uint64_t rootlink=link_queue.front();link_queue.pop();
			if(std::find(has_visited.begin(),has_visited.end(),rootlink)== has_visited.end()){
				has_visited.push_back(rootlink);
				clusterV.push_back(rootlink);
				//std::cout <<"circle"<<rootlink<<std::endl;
				alllinks=subClustering(rootlink,0);
				auto v=subClustering(rootlink,1);
				std::copy(v.begin(),v.end(),std::back_inserter(alllinks));
				std::vector<uint64_t> stv;
				std::sort(alllinks.begin(),alllinks.end());
				std::set_intersection(alllinks.begin(),alllinks.end(),m_linksList.begin(),m_linksList.end(),std::back_inserter(stv));
				for(const auto &id:stv){
					link_queue.push(id);
				}
			}
		}
		if(!clusterV.empty()){
			clusteringList.push_back(clusterV);
			count+=clusterV.size();
		}
	}
	//NOTICE("Clustering_size_"<<m_id<<":"<<count);
	//------------------------------------------
	std::vector<std::string> uuidlist;
	//先处理0或者1的uuid的聚类集合
	//split status
	std::map<std::string,std::vector<std::vector<uint64_t>>> splitMap;
	for(const auto &cv:clusteringList){
		std::set<std::string> uuidset;
		std::stringstream ss;
		ss<<"Clustering_team:["<<m_id<<"]";
		for(const auto &id:cv){
			ss<<id<<",";
			std::string uid=m_linksInfoList[id].uuid;
			if(!uid.empty()) uuidset.insert(uid);
		}
		ss<<"uuid_size:"<<uuidset.size();
		NOTICE(ss.str());
		if(uuidset.size()==0){
			getUidAndMakeCongestObj(uuidlist,cv,0,"");
		}else if(uuidset.size()==1){
			std::string key=*uuidset.begin();
			splitMap[key].push_back(cv);
#if 0
			int exist=0;
			if(std::find(uuidlist.begin(),uuidlist.end(),key)==uuidlist.end())
				exist=1;
			getUidAndMakeCongestObj(uuidlist,cv,exist,key);
#endif
		}
	}
	//处理分裂的情况.时间最久的集合使用该uuid
	time_t now=time(0);
	for(const auto &p:splitMap){
		time_t dst_sum=0;std::size_t index=0;
		//NOTICE("Split_clustering_"<<m_id<<",key="<<p.first<<",size="<<p.second.size());
		for(std::size_t i=0;i<p.second.size();i++){
			time_t sum=0;
			for(const auto &id:p.second[i]){
				sum+=(now-m_linksInfoList[id].begin_time);
			}
			if(sum>=dst_sum){
				index=i;dst_sum=sum;
			}
		}
		for(std::size_t i=0;i<p.second.size();i++){
			getUidAndMakeCongestObj(uuidlist,p.second[i],i==index?1:0,p.first);
		}
	}
	//然后处理>1的聚类集合
	for(const auto &cv:clusteringList){
		std::set<std::string> uuidset;
		for(const auto &id:cv){
			std::string uid=m_linksInfoList[id].uuid;
			if(!uid.empty()) uuidset.insert(uid);
		}
		if(uuidset.size()<=1) continue;
		for(auto it=uuidset.begin();it!=uuidset.end();)	{
			if(std::find(uuidlist.begin(),uuidlist.end(),*it)!=uuidlist.end())
				it=uuidset.erase(it);
			else
				++it;
		}
		if(uuidset.size()==0){
			getUidAndMakeCongestObj(uuidlist,cv,0,"");
		}else if(uuidset.size()==1){
			std::string key=*uuidset.begin();
			getUidAndMakeCongestObj(uuidlist,cv,1,key);
		}else{
			std::string ouid;
			std::time_t tmpt=time(0);
			for(const auto &xid:uuidset){
				std::time_t ct=m_congestObjList[xid]->getStarttime();
				if(tmpt>ct){
					tmpt=ct; 
					ouid=m_congestObjList[xid]->getkey();
				}
			}
			getUidAndMakeCongestObj(uuidlist,cv,1,ouid);
		}
	}
	//开始判断新增，持续，和结束的拥堵点
	congestClusteringProcess(uuidlist);
}
void provincesOfBusiness::preocessBeforeAction(const std::string &csid){
	uint64_t id=m_congestObjList[csid]->getMaxLengthLinkid();
	std::string name,admincode;
	//m_mapTool->getLinkNameAdmincode(id,name,admincode);
	if(admincode.empty())
		NOTICE("preocessBeforeAction:"<<id<<":"<<name<<":"<<admincode);
	m_congestObjList[csid]->setNameAdmincode(name,admincode);
}
void provincesOfBusiness::congestClusteringProcess(std::vector<std::string> &ulist){
	int size_1=ulist.size();
	std::sort(ulist.begin(),ulist.end());
	auto last=std::unique(ulist.begin(),ulist.end());
	ulist.erase(last,ulist.end());
	int size_2=ulist.size();
	if(size_1!=size_2){
		NOTICE("ERROR:::SIZE is NOT Equal.."<<m_id);
	}
	std::vector<std::string> vs;
	std::set_intersection(ulist.begin(),ulist.end(),m_congestList.begin(),m_congestList.end(),std::back_inserter(vs));
	//一直存在的
	for(const auto &csid:vs){
		//NOTICE("move_"<<m_id<<":"<<csid);
		//preocessBeforeAction(csid);
		m_congestObjList[csid]->on_move();
		//----其他业务
		for(const auto &pb:m_blist){
			pb->on_move(shared_from_this(),m_congestObjList[csid]);
		}

	}
	vs.clear();
	std::set_difference(ulist.begin(),ulist.end(),m_congestList.begin(),m_congestList.end(),std::back_inserter(vs));
	//新增的NewCongest
	for(const auto &csid:vs){
		//NOTICE("open_"<<m_id<<":"<<csid);
		//preocessBeforeAction(csid);
		m_congestObjList[csid]->on_open();
		for(const auto &pb:m_blist){
			pb->on_open(shared_from_this(),m_congestObjList[csid]);
		}
	}
	vs.clear();
	std::set_difference(m_congestList.begin(),m_congestList.end(),ulist.begin(),ulist.end(),std::back_inserter(vs));
	//可以释放的聚类
	for(const auto &csid:vs){
		//NOTICE("close_"<<m_id<<":"<<csid);
		m_congestObjList[csid]->on_close();
		for(const auto &pb:m_blist){
			pb->on_close(shared_from_this(),m_congestObjList[csid]);
		}

		m_congestObjList.erase(csid);
	}
	m_congestList=std::move(ulist);
	//NOTICE("congest_sum_"<<m_id<<":"<<m_congestList.size()<<"-"<<m_congestObjList.size());
}
void provincesOfBusiness::getUidAndMakeCongestObj(std::vector<std::string> &uall,const std::vector<uint64_t> &vlinks,int size,const std::string &uid){
	std::string xkey;
	if(size==0){
		std::shared_ptr<congestObj> co=std::make_shared<congestObj>(m_id,m_mapVersion);
		std::string key=co->getkey();
		m_congestObjList[key]=co;
		xkey=key;
	}else if(size==1){
		xkey=uid;
	}
	uall.push_back(xkey);
	for(const auto &id:vlinks){
		m_linksInfoList[id].setuid(xkey);
	}
	////
	std::vector<linksinfo> li;
	for(const auto &id:vlinks){
		li.push_back(m_linksInfoList[id]);
	}
	m_congestObjList[xkey]->setLinks(std::move(li));
}
void provincesOfBusiness::run(){
	time_t time_interval=time(0);
	char nsql[1024]	{0};
	const char *sql="update congest_summary set closed=1,close_type=5,end_time='%s' where province_id=%d and closed=0;";
	snprintf(nsql,1024,sql,CommonTools::unixtime2str(time_interval).c_str(),m_id);
	myDb::inst()->tryPut(nsql);
	while((time_interval/60)%2!=1){
		std::this_thread::sleep_for(std::chrono::seconds(10));
		time_interval=time(0);
	}
	time_interval=time_interval/60*60;
	while(!m_bFlag){
		if(m_mapChangedFlag){
			m_mapChangedFlag=false;
			m_mapTool=mapManagerInstance::inst()->m_mapManager;
		}
		time_t now =time(0);
		if(now > time_interval){
			if(getTrafficStatusData(time_interval)){
				//std::this_thread::sleep_for(std::chrono::seconds(10));
				time_interval+=120;
				continue;
			}
			//NOTICE("END============="<<m_id);
			makeCongestClustering();
			//NOTICE("END2============="<<m_id);
			time_interval+=120;
		}else{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	//get pb data
	//解析pb数据，获取拥堵的linkid
	//同时生成linksinfo对象，和vector中保存linkid
	//和之前保存的数据进行对比，如果之前没有则新建，如果有则赋值，如果现在没有则删除，保留最新的数据。
	//进行聚类操作
	//获取聚类出来的links集合比如有s1 ,s2 ,s3
	//查看以上集合中的linkid是否之前有聚类结果，最终统计为比如：s1之前聚类数据=0,s2=1,s3>1
	//如果为0则是新建congest—clustering对象，如果是1使用之前的聚类对象，如果>1则要判断
	//uuid已经在上一步使用，
	//最后如果存在0个则新建，存在1个则使用，存在多个则用时间最长的聚类对象。最终变为1
	//遍历操作结束
	//现在的vector<uuid>和之前的vector_old<uuid>做比较，如果-A---｜--B--｜--C---
	//A表示新的，走on_open函数，进行新建插入操作
	//B表示一直存在，走on_continue函数，走更新操作
	//C表示消亡的聚类对象，走on_close操作。走结束流程
	//然后走注册的流程：标签；tti；mapVersion等
	//
}
provincesOfBusiness::~provincesOfBusiness(){
	m_bFlag=true;
	m_pThread->join();
}
