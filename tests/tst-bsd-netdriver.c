#include <stdio.h>

#include <bsd/sys/net/if_var.h>
#include <bsd/sys/net/if.h>
#include <bsd/sys/net/if_arp.h>
#include <bsd/sys/net/ethernet.h>
#include <bsd/sys/net/if_dl.h>
#include <bsd/sys/net/if_types.h>
#include <bsd/sys/netinet/in.h>
#include <bsd/sys/netinet/in_var.h>
#include <bsd/sys/sys/sockio.h>
#include <bsd/sys/sys/socket.h>

/* Test log */
#define TLOG(...) printf(__VA_ARGS__)

/* Global ifnet */
struct ifnet* pifp;

static int
lge_ioctl(struct ifnet        *ifp,
          u_long          command,
          caddr_t         data)
{
    TLOG("lge_ioctl\n");
    return (0);
}

static void
lge_start(struct ifnet* ifp)
{
    TLOG("lge_start\n");
}

static void
lge_init(void *xsc)
{
    TLOG("lge_init\n");
}

int create_if(void)
{
    u_char eaddr[ETHER_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

    printf("[~] Creating interface!\n");
    pifp = if_alloc(IFT_ETHER);
    if (pifp == NULL) {
        printf("[-] if_alloc() failed!\n");
        return (-1);
    }

    if_initname(pifp, "test-net", 0);
    pifp->if_mtu = ETHERMTU;
    pifp->if_softc = (void*)"Driver private softc";
    pifp->if_flags = 0/* IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST */;
    pifp->if_ioctl = lge_ioctl;
    pifp->if_start = lge_start;
    pifp->if_init = lge_init;
    pifp->if_snd.ifq_maxlen = 2;
    pifp->if_capabilities = 0/* IFCAP_RXCSUM */;
    pifp->if_capenable = pifp->if_capabilities;

    ether_ifattach(pifp, eaddr);

    return (0);
}

void destroy_id(void)
{
    /* ether_ifdetach(pifp); */
    if_free(pifp);
}

void set_address(void)
{
    struct  sockaddr_in ia_addr;
    struct ifaddr ifa;
    bzero(&ifa, sizeof(ifa));
    bzero(&ia_addr, sizeof(ia_addr));

    ifa.ifa_addr = (struct sockaddr*)&ia_addr;

    ia_addr.sin_family = AF_INET;
    ia_addr.sin_len = sizeof(struct sockaddr_in);
    /* FIXME: use inet_addr when we have one */
    ia_addr.sin_addr.s_addr = 0xAABBCCDD;

    /* This causes the arp module issue a broadcast an arp tell packet
     * (Happen only if the interface was already up! */
    ether_ioctl(pifp, SIOCSIFADDR, (caddr_t)&ifa);

}

int main(void)
{
    TLOG("BSD Net Driver Test\n");

    create_if();

    /*
     * Let all domains know about this interface...
     * (There are non configured at this moment)
     */
    if_attachdomain(NULL);

    // set_address();
    destroy_id();

    TLOG("BSD Net Driver Test\n");
    return (0);
}

#undef TLOG
