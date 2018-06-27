QT       -= core gui

TARGET = cx
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH +=  ./../../

QMAKE_CXXFLAGS += /std:c++17

CX_BASE = ./../../

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


HEADERS += \
        $$CX_BASE/cx/container_of.hpp\
        $$CX_BASE/cx/core/allocators.hpp\
        $$CX_BASE/cx/core/basic_buffer.hpp\
        $$CX_BASE/cx/core/demangle.hpp\
        $$CX_BASE/cx/core/function.hpp\
        $$CX_BASE/cx/core/math.hpp\
        $$CX_BASE/cx/core/mp.hpp\
        $$CX_BASE/cx/core/tag.hpp\
        $$CX_BASE/cx/core/tmp.hpp\
        $$CX_BASE/cx/core/tuple_sample.hpp\
        $$CX_BASE/cx/core/type_list.hpp\
        $$CX_BASE/cx/cx.hpp\
        $$CX_BASE/cx/cxdefine.hpp\
        $$CX_BASE/cx/error.hpp\
        $$CX_BASE/cx/io/basic_buffer.hpp\
        $$CX_BASE/cx/io/basic_engine.hpp\
        $$CX_BASE/cx/io/basic_object.hpp\
        $$CX_BASE/cx/io/basic_read_op.hpp\
        $$CX_BASE/cx/io/basic_write_op.hpp\
        $$CX_BASE/cx/io/handler_op.hpp\
        $$CX_BASE/cx/io/internal/basic_implementation.hpp\
        $$CX_BASE/cx/io/internal/linux/epoll.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor_accept_op.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor_connect_op.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor_read_op.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor_socket_service.hpp\
        $$CX_BASE/cx/io/internal/reactor/reactor_write_op.hpp\
        $$CX_BASE/cx/io/internal/win32/completion_port.hpp\
        $$CX_BASE/cx/io/internal/win32/completion_port_accept_op.hpp\
        $$CX_BASE/cx/io/internal/win32/completion_port_connect_op.hpp\
        $$CX_BASE/cx/io/internal/win32/completion_port_socket_service.hpp\
        $$CX_BASE/cx/io/io.hpp\
        $$CX_BASE/cx/io/ip/basic_accept_context.hpp\
        $$CX_BASE/cx/io/ip/basic_accept_op.hpp\
        $$CX_BASE/cx/io/ip/basic_acceptor.hpp\
        $$CX_BASE/cx/io/ip/basic_address.hpp\
        $$CX_BASE/cx/io/ip/basic_connect_op.hpp\
        $$CX_BASE/cx/io/ip/basic_socket.hpp\
        $$CX_BASE/cx/io/ip/hdr.hpp\
        $$CX_BASE/cx/io/ip/ifaddr.hpp\
        $$CX_BASE/cx/io/ip/option.hpp\
        $$CX_BASE/cx/io/selector.hpp\
        $$CX_BASE/cx/service_repository.hpp\
        $$CX_BASE/cx/services.hpp\
        $$CX_BASE/cx/slist.hpp\
        $$CX_BASE/cx/stdext/removable_priority_queue.hpp\
        $$CX_BASE/cx/time/basic_timer.hpp\
        $$CX_BASE/cx/time/internal/reactor_timer_fd_service.hpp\
        $$CX_BASE/cx/time/internal/win32_timer_queue_service.hpp\
        $$CX_BASE/cx/vision/bitmap.hpp\
        $$CX_BASE/cx/vision/canny.hpp\
        $$CX_BASE/cx/vision/fft.hpp\
        $$CX_BASE/cx/vision/harris.hpp\
        $$CX_BASE/cx/vision/hough.hpp\
        $$CX_BASE/cx/vision/image.hpp\
        $$CX_BASE/cx/vision/image_draw.hpp\
        $$CX_BASE/cx/vision/image_proc.hpp\
        $$CX_BASE/cx/vision/kernel.hpp\
        $$CX_BASE/cx/vision/sobel.hpp\
        $$CX_BASE/cx/vision/vision.hpp

SOURCES += \
        $$CX_BASE/cx/cx.cpp\
        $$CX_BASE/cx/vision/bitmap.cpp\
        $$CX_BASE/cx/vision/canny.cpp\
        $$CX_BASE/cx/vision/fft.cpp\
        $$CX_BASE/cx/vision/harris.cpp\
        $$CX_BASE/cx/vision/hough.cpp\
        $$CX_BASE/cx/vision/image.cpp\
        $$CX_BASE/cx/vision/image_draw.cpp\
        $$CX_BASE/cx/vision/image_proc.cpp



unix {
    target.path = /usr/lib
    INSTALLS += target
}
