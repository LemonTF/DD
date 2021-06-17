//
//  CoorManager.cpp
//  thriftDemo
//
//  Created by LiuGuoping on 16/12/15.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#include "CoorManager.h"

CoorManager::CoorManager(){
    coor_body_buf_ = NULL;
	total_buf=NULL;
}

CoorManager::~CoorManager(){
	if(total_buf){
		delete total_buf;
		std::cout <<"delete Coor total_buf\n";
		total_buf=NULL;
	}
    coor_body_buf_ = NULL;
}

bool CoorManager::LoadCoorData(const char* cache_path, uint32_t & partition_code){
    FILE* file = fopen(cache_path, "rb");
	if(file==NULL)
		return false;
	// 偏移至文件尾部
    fseeko(file, 0, SEEK_END);
    // 获取文件的大小
    uint64_t file_size = ftello(file);
    // 偏移回文件头部位置
    fseeko(file, 0, SEEK_SET);
    //
    total_buf = new char[file_size];
    if(!total_buf) {
        std::cerr<<"calc: load data malloc failed"<<std::endl;
        return false;
    }
    // 读取整个文件至内存中，第三个参数为缓冲区大小，即每次多去的 buffer 的大小
    fread(total_buf, file_size, 1, file);
    fclose(file);
    file = NULL;
    
    uint64_t offset=0;
    char magiccode[32]={0};
    // 协议的前 32 个字节是 magiccode
    memcpy(magiccode, total_buf, 32);
    offset+=32;
    printf("LoadCoor magiccode:%s\n",magiccode);
    // 再拷贝 64 位 并顺序转换为 size
    uint64_t save_size=0;
    memcpy(&save_size, total_buf+offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    printf("LoadCoor totaloffset:%lu\n",save_size);
    // 再拷贝32 位作为 partition_code
    uint32_t partition_code_file=0xFFFFFFFF;
    memcpy(&partition_code_file, total_buf+offset, sizeof(uint32_t));
    offset+=sizeof(uint32_t);
    printf("LoadCoor partition_code:%u\n",partition_code_file);
    // 再拷贝32 位作为 version_code
    char save_version[32] = {0};
    memcpy(save_version, total_buf + offset, 32);
    offset += 32;
    printf("LoadCoor save_version:%s\n",save_version);
    // 返回了 body 的 offset
    coor_body_buf_ = total_buf + offset;

    return true;
}

bool CoorManager::GetRoadPoints(uint64_t coor_offset, std::vector<PointCoor> & point_vec, bool reverse){
    if(!coor_body_buf_){
        return false;
    }
    RoadCoor* coor_info =(RoadCoor*)(coor_body_buf_+coor_offset);
    uint32_t pointnum=coor_info->point_num;
    PointCoor* coor_list=coor_info->ListCoor();
    if(reverse){
        for (int loop=pointnum-1;loop>=0; loop-- ){
            PointCoor cur_coor=coor_list[loop];
            point_vec.push_back(cur_coor);
        }
        return true;
    }
    else{
        for (int loop=0;loop<pointnum; loop++ ){
            PointCoor cur_coor=coor_list[loop];
            point_vec.push_back(cur_coor);
        }
        return true;
    }
}
