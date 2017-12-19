recv_t : recv_t.cpp ./mylibrtmp/amf_librtmp.h ./mylibrtmp/amf_librtmp.c ./mylibrtmp/message.h ./mylibrtmp/message.cpp \
        ./mylibrtmp/sendengine.h ./mylibrtmp/sendengine.cpp
        g++ recv_t.cpp ./mylibrtmp/amf_librtmp.h ./mylibrtmp/amf_librtmp.c \
        ./mylibrtmp/message.h ./mylibrtmp/message.cpp \
        ./mylibrtmp/sendengine.h ./mylibrtmp/sendengine.cpp  -o recv_t -g -std=c++11
