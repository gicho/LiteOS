#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "ethernetif.h"
#include "stdint.h"
#include <stdio.h>
#include "lwip/timeouts.h"
#include "lwip/priv/tcp_priv.h"
#include "ethernetif.h"

#include "los_bsp_lwip.h"

#include "httpserver-netconn.h"

struct netif netif;
int errno = 0;

void LOS_LwipAppInit(void)
{
    //shell_init();
    http_server_netconn_init();
}

void LOS_EvbLwipInit(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    /* init the tcpip */
    tcpip_init( NULL, NULL );
    
    /* initializes the dynamic memory heap defined by MEM_SIZE */
    mem_init();

    /* initializes the memory pools defined by MEMP_NUM_x */
    memp_init();
  
#ifdef TIMEOUT_CHECK_USE_LWIP
    sys_timeouts_init();
#endif /* TIMEOUT_CHECK_USE_LWIP */
  
#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    IP4_ADDR(&ipaddr, 192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255 , 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

#endif /* USE_DHCP */

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
              struct ip_addr *netmask, struct ip_addr *gw,
              void *state, err_t (* init)(struct netif *netif),
              err_t (* input)(struct pbuf *p, struct netif *netif))
      
     Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, tcpip_input);

    /* registers the default network interface */
    netif_set_default(&netif);

    /* when the netif is fully configured this function must be called */
    netif_set_up(&netif);
    
    LOS_LwipAppInit();
}

/*****************************************************************************
 Function    : lwip_pkt_handle
 Description : handle lwip package, send eth data to lwip thread
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void lwip_pkt_handle(void)
{
    /* read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    ethernetif_input(&netif);
}
