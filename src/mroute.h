/* Generic kernel multicast routing API for Linux and *BSD */
#ifndef SMCROUTE_MROUTE_H_
#define SMCROUTE_MROUTE_H_

#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>

#ifdef HAVE_LINUX_MROUTE_H
#define _LINUX_IN_H             /* For Linux <= 2.6.25 */
#include <linux/types.h>
#include <linux/mroute.h>
#endif

#ifdef HAVE_LINUX_MROUTE6_H
#include <linux/mroute6.h>
#endif

#ifdef HAVE_LINUX_FILTER_H
#include <linux/filter.h>
#endif

#ifdef HAVE_NET_ROUTE_H
#include <net/route.h>
#endif

#ifdef HAVE_NETINET_IP_MROUTE_H
#include <netinet/ip_mroute.h>
#else
# ifdef __APPLE__
#  include "ip_mroute.h"
# endif
#endif

#ifdef HAVE_NETINET6_IP6_MROUTE_H
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <netinet6/ip6_mroute.h>
#endif

#include "queue.h"

#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)
#endif

/*
 * IPv4 multicast route
 */
#ifndef MAXVIFS
#define MAXVIFS 32
#endif

#define MAX_MC_VIFS MAXVIFS		/* from linux/mroute.h */

struct mroute4 {
	LIST_ENTRY(mroute4) link;

	struct in_addr sender;
	struct in_addr group;           /* multicast group */
	short          len;		/* prefix len, or 0:disabled */

	short          inbound;         /* incoming VIF    */
	uint8_t        ttl[MAX_MC_VIFS];/* outgoing VIFs   */
};

/*
 * IPv6 multicast route
 */
#ifdef HAVE_IPV6_MULTICAST_ROUTING
#ifndef MAXMIFS
#define MAXMIFS 32
#endif

#define MAX_MC_MIFS MAXMIFS		/* from linux/mroute6.h */
#else
#define MAX_MC_MIFS 1			/* Dummy value for builds w/o IPv6 routing */
#endif

struct mroute6 {
	struct sockaddr_in6 sender;
	struct sockaddr_in6 group;      /* multicast group */
	short   inbound;                /* incoming VIF    */
	uint8_t ttl[MAX_MC_MIFS];       /* outgoing VIFs   */
};

/*
 * Generic multicast route (wrapper for IPv4/IPv6 mroute)
 */
struct mroute {
	int version;		/* 4 or 6 */
	union {
		struct mroute4 mroute4;
		struct mroute6 mroute6;
	} u;
};

/*
 * Raw IGMP socket used as interface for the IPv4 mrouted API.
 * Receives IGMP packets and upcall messages from the kernel.
 */
extern int mroute4_socket;

/*
 * Raw ICMPv6 socket used as interface for the IPv6 mrouted API.
 * Receives MLD packets and upcall messages from the kenrel.
 */
extern int mroute6_socket;

int  mroute4_enable    (int do_vifs);
void mroute4_disable   (void);
int  mroute4_dyn_add   (struct mroute4 *mroute);
void mroute4_dyn_flush (void);
int  mroute4_add       (struct mroute4 *mroute);
int  mroute4_del       (struct mroute4 *mroute);

int  mroute6_enable    (int do_vifs);
void mroute6_disable   (void);
int  mroute6_add       (struct mroute6 *mroute);
int  mroute6_del       (struct mroute6 *mroute);

int  mroute_add_vif    (char *ifname, uint8_t threshold);
int  mroute_del_vif    (char *ifname);

#endif /* SMCROUTE_MROUTE_H_ */
