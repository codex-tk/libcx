/**
 */
#ifndef __cx_io_ip_ifaddr_h__
#define __cx_io_ip_ifaddr_h__

#include <cx/io/io.hpp>

#include <vector>
#include <iostream>

namespace cx::io::ip {

    struct ifaddr {
        std::string name;
        std::string ip_address;
        std::string mask;
        std::string gateway;
    };

    std::vector< ifaddr > interfaces( void ) {
        std::vector< ifaddr > ifaddrs;
#if CX_PLATFORM == CX_P_WINDOWS 
        // https://msdn.microsoft.com/en-us/library/aa365917.aspx GetAdaptersInfo function
        ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
        PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *) std::malloc(sizeof (IP_ADAPTER_INFO));
        PIP_ADAPTER_INFO pAdapter = NULL;

        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
            std::free(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO *) std::malloc(ulOutBufLen);
            if (pAdapterInfo == NULL) {
                return ifaddrs;
            }
        }
        DWORD dwRetVal = 0;
        if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
            pAdapter = pAdapterInfo;
            while ( pAdapter ) {
                switch (pAdapter->Type) {
                case MIB_IF_TYPE_ETHERNET:
                //case IF_TYPE_IEEE80211: wifi and bluetooth
                    {
                        char buf[1024] = {0 , };
                        ifaddr addr;
                        addr.name = if_indextoname(pAdapter->Index , buf);
                        addr.ip_address = pAdapter->IpAddressList.IpAddress.String;
                        addr.mask = pAdapter->IpAddressList.IpMask.String;
                        addr.gateway = pAdapter->GatewayList.IpMask.String;
                        ifaddrs.push_back(addr);
                    }
                    break;
                case MIB_IF_TYPE_OTHER:
                case MIB_IF_TYPE_TOKENRING:
                case MIB_IF_TYPE_FDDI:
                case MIB_IF_TYPE_PPP:
                case MIB_IF_TYPE_LOOPBACK:
                case MIB_IF_TYPE_SLIP:
                default:
                    break;
                }
                pAdapter = pAdapter->Next;
               
            }
        }
#else

#endif
        return ifaddrs;
    }

}

#endif