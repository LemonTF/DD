bin_PROGRAMS=congestion_clustering

AM_SOURCES=business.cpp CommonTools.cpp config_file.cpp congestObject.cpp  LogUtils.cpp main.cpp MapManager.cpp md5.cpp mydb.cpp overallDataManagement.cpp provinceOfBusiness.cpp  TrafficMeta.pb.cc
AM_CPPFLAGS=-Wall -lpthread -m64 -g -std=c++11 -I./ -I$(prefix)/include  -I ~/include/f3sdk
AM_LDFLAGS=-Wall -lpthread -m64 -L$(prefix)/lib -L/usr/local/mysql/lib -L/usr/local/lib64 -L/usr/lib64/mysql/ -L ~/lib

congestion_clustering_SOURCES=$(AM_SOURCES)
congestion_clustering_CPPFLAGS=$(AM_CPPFLAGS)
congestion_clustering_LDFLAGS=$(AM_LDFLAGS) -lrt -lmysqlclient -lcurl -lprotobuf -lprotoc -lz -llog4cplus -ljson -luuid -lgeos -lf3sdk
