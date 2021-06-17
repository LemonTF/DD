//
//  TopoManager.cpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#include "TopoManager.h"

TopoManager::TopoManager(){
    topo_info_list_ = NULL;
    topo_info_size_ = 0;
	total_buf=NULL;
}

TopoManager::~TopoManager(){
    if(topo_info_list_){
//		for(uint32_t i=0; i<topo_info_size_; i++){
//			if(topo_info_list_[i]){
//				delete topo_info_list_[i];
//				topo_info_list_[i]=NULL;
//			}
//		}
        delete topo_info_list_;
        topo_info_list_ = NULL;
        std::cerr<<"destruct topo manager"<<std::endl;
    }
	if(total_buf){
		delete total_buf;
		std::cout << "delete Topo total_buf"<<std::endl;
		total_buf=NULL;
	}
}


bool TopoManager::LoadTopoData(const char* cache_path, uint32_t & partition_code){
    FILE* file = fopen(cache_path, "rb");
	if(file==NULL) return false;
    fseeko(file, 0, SEEK_END);
    uint64_t file_size = ftello(file);
    fseeko(file, 0, SEEK_SET);
    
    total_buf = new char[file_size];
    if(!total_buf) {
        std::cerr<<"calc: load data malloc failed"<<std::endl;
        return false;
    }
    fread(total_buf, file_size, 1, file);
    fclose(file);
    file = NULL;
    
    uint64_t offset=0;
    char magiccode[32]={0};
    memcpy(magiccode, total_buf, 32);
    offset+=32;
    printf("LoadTopo magiccode:%s\n",magiccode);
    
    uint64_t save_size=0;
    memcpy(&save_size, total_buf+offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    printf("LoadTopo totaloffset:%lu\n",save_size);
    
    uint32_t partition_code_file=0xFFFFFFFF;
    memcpy(&partition_code_file, total_buf+offset, sizeof(uint32_t));
    offset+=sizeof(uint32_t);
    printf("LoadTopo partition_code:%u\n",partition_code_file);
    
    char save_version[32] = {0};
    memcpy(save_version, total_buf + offset, 32);
    offset += 32;
    printf("LoadTopo save_version:%s\n",save_version);
    
    uint32_t toposize=0;
    memcpy(&toposize, total_buf+offset, sizeof(uint32_t));
    offset+=sizeof(uint32_t);
    printf("LoadTopo toposize:%u\n",toposize);
    // 返回 拓扑 的 size
    topo_info_size_=toposize;   //topo size
    
    char *body_buf=total_buf+offset;
    
    offset=0;
    
    //topo_info_list_ = new TOPOINFO*[topo_info_size_];
    topo_info_list_ = new TOPOINFO[topo_info_size_];
    
    for (uint32_t index=0; index<toposize; index++){
        //topo_info_list_[index]= new TOPOINFO();
        Topo *topo = (Topo *)(body_buf+offset);
        topo_info_list_[index].setTopo(topo);
        topo_info_list_[index].pre_links=topo->ListI();
        topo_info_list_[index].next_links=topo->ListO();
        
        offset+=sizeof(Topo)+(topo_info_list_[index].in_link_cnt + topo_info_list_[index].out_link_cnt)* sizeof(TopoLink);
    }
    printf("success load topo list\n");
    return true;
}

// 获取road index, 必须加载road文件，才有意义。
bool TopoManager::GetLinkIndex(uint32_t topo_index, uint32_t& link_index){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        link_index = topo_info.road_index;
        return true;
    }
}

// 获取入度topo的topoindex的vector
bool TopoManager::GetPreTopoIndexs(uint32_t topo_index, std::vector<uint32_t>& pre_topo_indexs){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        if (NULL == topo_info.pre_links){
            return false;
        }
        for(int i=0; i<topo_info.in_link_cnt; i++){
            pre_topo_indexs.push_back(topo_info.pre_links[i].topo_index);
        }
        return true;
    }
}
// 获取入度topo的linkindex的vector
bool TopoManager::GetPreLinkIndexs(uint32_t topo_index, std::vector<uint32_t>& pre_link_indexs){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        if (NULL == topo_info.pre_links){
            return false;
        }
        std::vector<uint32_t> pre_topo_indexs;
        if(!GetPreTopoIndexs(topo_index, pre_topo_indexs)){
            return false;
        }
        for(std::size_t i=0; i<pre_topo_indexs.size(); i++){
            uint32_t pre_topo_index=pre_topo_indexs[i];
            uint32_t pre_link_index;
            if(!GetLinkIndex(pre_topo_index, pre_link_index)){
                return false;
            }
            pre_link_indexs.push_back(pre_link_index);
        }
        return true;
    }
}

// 获取出度topo的topoindex的vector
bool TopoManager::GetNextTopoIndexs(uint32_t topo_index, std::vector<uint32_t>& next_topo_indexs){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        if (NULL == topo_info.next_links){
            return false;
        }
        for(int i=0; i<topo_info.out_link_cnt; i++){
            next_topo_indexs.push_back(topo_info.next_links[i].topo_index);
        }
        return true;
    }
}
// 获取出度topo的linkindex的vector
bool TopoManager::GetNextLinkIndexs(uint32_t topo_index, std::vector<uint32_t>& next_link_indexs){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        if (NULL == topo_info.next_links){
            return false;
        }
        std::vector<uint32_t> next_topo_indexs;
        if(!GetNextTopoIndexs(topo_index, next_topo_indexs)){
            return false;
        }
        for(std::size_t i=0; i<next_topo_indexs.size(); i++){
            uint32_t next_topo_index=next_topo_indexs[i];
            uint32_t next_link_index;
            if(!GetLinkIndex(next_topo_index, next_link_index)){
                return false;
            }
            next_link_indexs.push_back(next_link_index);
        }
        return true;
    }
}

bool TopoManager::GetDirection(uint32_t topo_index, int& dir){
    if(!topo_info_list_ || topo_index>=topo_info_size_){
        return false;
    }
    else{
        TOPOINFO topo_info =topo_info_list_[topo_index];
        dir = topo_info.direction;
        return true;
    }
    return true;
}

