/*! \file
    \brief Definition of static helper functions for the ipport_aview_t

\par Description of the listen_aview logic - problem description
- due to NAT, the local view and the public view of a listen ipport_addr_t
  are not the same.
  - the ipport_aview_t object is made to handle this case.
- Moreover some NAT doesnt support loopback, so the destination ipport_addr_t
  toward a host may be different depending if the remote host is outside
  or inside the NAT realm of this host.
  - say host1 and host2 are behind the same NAT realm and host3 is outside of it
    - the internal ip_addr_t of host1 is 10.0.0.1 and the public address of the 
      most outter NAT box is 1.2.3.4
    - say this NAT realm is not loopback_ok
    - say that the most outter NAT box has a port forward 1.2.3.4:5000 -> 10.0.0.1:4000
  - host2 CAN NOT reach host1 by using 1.2.3.4:5000
    - it would require the NAT box to perform a loopback and it doesnt support it
  - but host3 CAN reach host1 by using the 1.2.3.4:5000
  - as a consequence, host2 MUST be aware that host1 is behind the same NAT realm
    and use the 10.0.0.1:4000 as destination address toward host1

\par Description of the listen_aview logic - type of publication direct or indirect
- in all case, host1 MUST publish its listen address
- there are 2 cases of publications
  1. if the publication is done directly from the host1 to the querier host
     - e.g. published on a LAN e.g. nslan_peer_t
  2. if the publication is done indirectly, using intermediary, to the querier host
     - e.g. published thru a DHT or bt http tracker
- with direct publication, the publisher knows the querier location by using 
  the remote ipport of the connection.
  - the publisher uses this information in order to provide the suitable listen_addr
    (local view or public view) depending on the querier location. (the exact
    logic used in this case is described below in 'direct publication details')
  - if the remote ipport of the connection is non public, then the other host is 
    assumed within the same NAT realm
  - if the remote ipport of the connection is public, then the other host is assumed
    outside the NAT realm
  - ASSUMPTION: this is not true if the NAT is operating with public address inside.
    - on the other side is VERY rarely seen in the field and is discouraged by the IETF.
    - so we considere it 'negligible'
- with indirect publication, the publisher ignores the location of the querier
  and so can not take advantage of this information. 
  - to publish indirectly a record with the public view of the listen address
    works without trouble if the querier is ousider the publisher NAT realm.
    but this fails if the querier is inside the NAT realm and if the NAT realm is
    not loopback_ok.
  - to fix this, several possibilities:
    1. in the published record, put the public view and all the possible local view
       - aka get all the network interfaces ip addresses of the publisher
         and mix them with the local view of the listen_addr
        - the querier get this record and attemps to connect all the address it 
          contains.
        - this works in both cases, i.e. with querier inside and outside the same NAT realm
        - e.g. ntudp uses exactly this strategy 
        - BUT this requires to attemps many connections while it is logically only one
          - thus the code is modified and appears unusual.
          - ntudp performs this operation transparently to hide this 'unusual'
            aspect from the caller
    2. publish only the public view of the listen address for the hosts outside the 
       NAT realm and provide the local view another source for local listen_addr, using 
       the LAN, for the hosts inside the NAT realm. 
       - provide the ability to find the record for the host1 from the LAN 
         (e.g. via nslan_peer_t)
       - e.g. neoip_bt uses exactly this strategy.
       - ASSUMPTION: all peers behind the same NAT realm are on the same LAN
         and reachable via ethernet broadcast/multicast
         - this is not guaranteed at all. 
         - in the field some network topology have very large domain behind
           a single NAT realm. and this large domain is seperated in mutiple LAN
         - nevertheless almost all homeusers which are behind NAT due to their
           ASDL modem, have a single LAN
         - so it is considered 'acceptable'... but clearly not perfect

\par Description of the listen_aview logic - direct publication details
- the network location of host1, host2, host3 are the same as given previously.
  - host1 is always the publisher, and host2/host3 are always queriers
- there are 2 actions :
  1. building the listen addr: done by the publisher host1 to sends to the querier
     host (which may be host2 or host3)
  2. parsing the listen addr: done by the querier to produce the actual destination
     address for it to reach host1
- terminology and assumption: 
  - xmit_listen_ipport : the listen address which is actually sent over the network
    - if it is a null ipport_addr_t, the publisher is not inetreach_ok
    - it it is non null, it MUST have a nonnull port
    - if it is non null, the ipaddr may be is_any() or is_fully_qualified()
      - but MUST NOT be null.
  - listen_aview.lview(), once setup
    - MUST NOT be null
    - the port MUST be non-null
    - the ipaddr MUST be either any or fully qualified
  - listen_aview.pview(), once setup
    - if it is null, it means that host1 is inetreach_ok
    - it is is not null, it means that host1 is NOT inetreach_ok
    - if it is not null, the port MUST be non null
    - if it is not null, the ipaddr MUST be is_any() or is_fully_qualified()
  - the destination and source ipaddress obtained from the connection MUST be
    is_fully_qualified()
- BUILDING: aims to produce a xmit_listen_ipport from listen_aview and for a dst_ipport 
  - it is done by host1 and requires to know the listen ipport_aview_t of host1
    and the destination ip address of the querier
    - the destination ip address of the querier is simply the remote ip address
      of the connection uses to perform the query. it is ok as it is only
      for direct publication.
  - if the dest ipaddr is non public, the sent xmit_listen_ipport is listen_aview.lview()
    - the dest_ipaddr may be a loopback one (127.0.0.1/8) or a private one (rfc1912)
      or a linklocal one(169.254.0.0/16)
    - the queried is assumed behind the same NAT realm
    - host1 being inetreach_ok or not is irrelevant in this case as they are both
      behind the same NAT realm
  - if the dest ipaddr is public, and host1 is inetreach_ok, the sent xmit_listen_ipport
    is the listen_aview.pview()
  - if the dest ipaddr is public, and host1 is NOT inetreach_ok, the sent xmit_listen_ipport
    is a null ipport_addr_t
- PARSING: aims to produce a dst_ipport toward xmit_listen_ipport sender with the src_ipport 
  - if the recved xmit_listen_ipport is null, the sender declared itself unreachable
    by the local host, so return a null ipport_addr_t
  - if the xmit_listen_ipport.ipaddr() is_fully_qualified(), return the xmit_listen_ipport itself
  - if the xmit_listen_ipport.ipaddr() is_any(), remplace it by the src_ipport.ipaddr()
    in xmit_listen_ipport and return the resulting ipport_addr_t.

\par Description of the listen_aview logic - direct publication example
- EXAMPLE: in case of 2 hosts behind the same realms
  - host2 sends a query to host1 thru a connection
  - host1 read the source ip address of this connection
  - host1 find out that host2 ip address is a non-public one
  - following the building algo, host1 replies its listen_aview.lview() to host2
  - host2 receives the reply and extract the remote ip address of the connection
  - following the parsing algo, host2 determines that it can reach host1 via
    - xmit_listen_ipport itself if xmit_listen_ipport.ipaddr().is_fully_qualified()
    - or by ipport_addr_t(src_ipport.ipaddr(), xmit_listen_ipport.port()) if 
      xmit_listen_ipport.ipaddr().is_any()
  - in all cases, host2 ends up with the proper destination address for host1
- EXAMPLE: in case of querier not inside the publisher most outter NAT realm
  - host2 sends a query to host1 thru a connection
  - host1 read the source ip address of this connection
  - host1 find out that host2 ip address is a public one
  - following the building algo:
    - if host1 is inetreach_ok, host1 replies its listen_aview.pview() to host2
    - if host1 is not inetreach_ok, host1 replies a null ipport_addr_t()
  - host2 receives the reply and extract the remote ip address of the connection
  - following the parsing algo, host2 determines that it can reach host1 via
    - xmit_listen_ipport itself if xmit_listen_ipport.ipaddr().is_fully_qualified()
    - or by ipport_addr_t(src_ipport.ipaddr(), xmit_listen_ipport.port()) if 
      xmit_listen_ipport.ipaddr().is_any()
    - or if xmit_listen_ipport is null, host2 can not reach host1    
  - in all cases, host2 ends up with the proper destination address for host1.
    - it should be noted that host2 has to test if the host1 is reachable or 
      not as it may not be.

\par about listen_aview.lview().ipaddr() being is_any() or is_fully_qualified()
- both case are allowed, nevertheless they being is_any() provides several advantages
- with direct publication, being is_any() allows to publish a single record
  in all cases.
  - the publisher may be on various LAN at the same time
    - e.g. on lo/loopback, or several ethernet such as wired one and a wifi one
  - if the local ip address is any, the publisher only send a single version
    of the xmit_listen_ipport and works in all cases.
  - if the local ip address is fully qualified, the querier will be required
    to have access to it. and this forces the publisher to either be reachable
    on a single LAN or to handle each LAN separatly
  - so being is_any() offers simpler codes and more flexibility in the field
- with indirect publication, being is_any() may requires to provide one listen
  ipport for each connected interface.
  - so to discover the ip address of all those interface 
    - it is rather low level from a OS point of view so specific to each OS
    - so it may produce code portability issue 
  - or to hardcode to handle a single network interface which may be easier.
    - e.g. discover the ip address of the local interface thru the socket API
      1. connect a socket to the public internet (anywhere)
      2. do a getpeername() to get the local address of this socket.

*/

/* system include */
/* local include */
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			tcp_listen_aview_from_conf
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get a free listen ipport from the lib_session_t config (without actually binding it)
 * 
 * @return in case of error, the view.first is a null ipport_addr_t
 */
ipport_aview_t	ipport_aview_helper_t::tcp_listen_aview_from_conf()	throw()
{
	const strvar_db_t &session_conf	= lib_session_get()->session_conf();
	ip_addr_t	lview_addr	= session_conf.get_first_value("tcp_listen_lview_addr","0.0.0.0");
	ip_addr_t	pview_addr	= session_conf.get_first_value("tcp_listen_pview_addr","0.0.0.0");
	uint16_t	lview_port_min	= atoi(session_conf.get_first_value("tcp_listen_lview_port_min","0").c_str());
	uint16_t	lview_port_max	= atoi(session_conf.get_first_value("tcp_listen_lview_port_max","0").c_str());
	uint16_t	pview_port_min	= atoi(session_conf.get_first_value("tcp_listen_pview_port_min","0").c_str());
	uint16_t	pview_port_max	= atoi(session_conf.get_first_value("tcp_listen_pview_port_max","0").c_str());
	uint16_t	chosen_lport	= 0;

	// log to debug
	KLOG_ERR("lview_ipaddr=" << lview_addr	<< " lview_port_min="	<< lview_port_min
						<< " lview_port_max="	<< lview_port_max);
	KLOG_ERR("pview_ipaddr=" << pview_addr	<< " pview_port_min="	<< pview_port_min
						<< " pview_port_max="	<< pview_port_max);

	// sanity check - lview_port_min/max MUST be either both configured or none are
	if( (!lview_port_max) != (!lview_port_min) ){
		KLOG_ERR("ERROR in the neoip_session.conf file. either tcp_listen_lview_port_min and tcp_listen_lview_port_max are BOTH configured, or NONE are.");
		EXP_ASSERT( 0 );
	}
	// sanity check - pview_port_min/max MUST be either both configured or none are
	if( (!pview_port_max) != (!pview_port_min) ){
		KLOG_ERR("ERROR in the neoip_session.conf file. either tcp_listen_lview_port_min and tcp_listen_pview_port_max are BOTH configured, or NONE are.");
		EXP_ASSERT( 0 );
	}
	// sanity check - pview_port range MUST be as large as lview_port range
	if( pview_port_min && pview_port_min && lview_port_max-lview_port_min != pview_port_max-pview_port_min){
		KLOG_ERR("ERROR in lib_session.conf. tcp_listen_pview port range is not the same size as the lview port range.");
		EXP_ASSERT( 0 );
	}

	// if there is a lview_port range, try to bind within this range
	if( lview_port_min ){
		for(uint16_t cur_port = lview_port_min; cur_port <= lview_port_max; cur_port++){
			ipport_addr_t	listen_ipport(lview_addr, cur_port);
			inet_err_t	inet_err;
			tcp_resp_t *	tcp_resp;
			// try to start a tcp_resp_t on the cur_ipport
			tcp_resp	= nipmem_new tcp_resp_t();
			inet_err	= tcp_resp->start(listen_ipport, NULL, NULL);
			nipmem_zdelete	tcp_resp;
			// if the start() failed, goto the next
			if( inet_err.failed() )	continue;
			// if the binding succeed, set the cur_port and leave the loop
			chosen_lport	= cur_port;
			break;	
		}
	}
	
	// if there are no lview_port range or if bind within it failed, bind dynamicaly 
	// and return a null pview
	if( !lview_port_min || chosen_lport == 0 ){
		ipport_addr_t	listen_ipport(lview_addr, 0);
		inet_err_t	inet_err;
		tcp_resp_t *	tcp_resp;
		// try to start a tcp_resp_t on the cur_ipport
		tcp_resp	= nipmem_new tcp_resp_t();
		inet_err	= tcp_resp->start(listen_ipport, NULL, NULL);
		DBG_ASSERT( !inet_err.failed() );
		// update the listen_ipport with the bound address
		listen_ipport	= tcp_resp->get_listen_addr();
		// delete the just created tcp_resp_t
		nipmem_zdelete	tcp_resp;
		// log to debug
		if( lview_port_min )	KLOG_INFO("unable to bind within configured lview_port_min/max range, falling back on dynamic allocation.");
		// return the result to the caller with a null pview
		return ipport_aview_t().lview(listen_ipport);
	}


	// sanity check - here, chosen_lport has been allocated within configured lview_port range 
	DBG_ASSERT( chosen_lport >= lview_port_min && chosen_lport <= lview_port_max );
	DBG_ASSERT( lview_port_min && lview_port_max );


	// build the result
	ipport_aview_t	listen_aview;
	// set the listen_aview.lview()
	listen_aview.lview	( ipport_addr_t(lview_addr, chosen_lport) );
	// set the listen_aview.pview() IIF there is a configured pview_port range
	if( pview_port_min ) 	listen_aview.pview( ipport_addr_t(pview_addr, pview_port_min+(chosen_lport-lview_port_min)) );
	// return the just built listen_aview
	return listen_aview;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         udp_listen_aview_from_conf
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get a free udp listen ipport_aview_t from the lib_session_t config (without actually binding it)
 * 
 * @return in case of error, the view.first is a null ipport_addr_t
 */
ipport_aview_t	ipport_aview_helper_t::udp_listen_aview_from_conf()	throw()
{
	const strvar_db_t &session_conf	= lib_session_get()->session_conf();
	ip_addr_t	lview_addr	= session_conf.get_first_value("udp_listen_lview_addr","0.0.0.0");
	ip_addr_t	pview_addr	= session_conf.get_first_value("udp_listen_pview_addr","0.0.0.0");
	uint16_t	lview_port_min	= atoi(session_conf.get_first_value("udp_listen_lview_port_min","0").c_str());
	uint16_t	lview_port_max	= atoi(session_conf.get_first_value("udp_listen_lview_port_max","0").c_str());
	uint16_t	pview_port_min	= atoi(session_conf.get_first_value("udp_listen_pview_port_min","0").c_str());
	uint16_t	pview_port_max	= atoi(session_conf.get_first_value("udp_listen_pview_port_max","0").c_str());
	uint16_t	chosen_lport	= 0;

	// log to debug
	KLOG_ERR("lview_ipaddr=" << lview_addr	<< " lview_port_min="	<< lview_port_min
						<< " lview_port_max="	<< lview_port_max);
	KLOG_ERR("pview_ipaddr=" << pview_addr	<< " pview_port_min="	<< pview_port_min
						<< " pview_port_max="	<< pview_port_max);

	// sanity check - lview_port_min/max MUST be either both configured or none are
	if( (!lview_port_max) != (!lview_port_min) ){
		KLOG_ERR("ERROR in the neoip_session.conf file. either udp_listen_lview_port_min and udp_listen_lview_port_max are BOTH configured, or NONE are.");
		EXP_ASSERT( 0 );
	}
	// sanity check - pview_port_min/max MUST be either both configured or none are
	if( (!pview_port_max) != (!pview_port_min) ){
		KLOG_ERR("ERROR in the neoip_session.conf file. either udp_listen_lview_port_min and udp_listen_pview_port_max are BOTH configured, or NONE are.");
		EXP_ASSERT( 0 );
	}
	// sanity check - pview_port range MUST be as large as lview_port range
	if( pview_port_min && pview_port_min && lview_port_max-lview_port_min != pview_port_max-pview_port_min){
		KLOG_ERR("ERROR in lib_session.conf. udp_listen_pview port range is not the same size as the lview port range.");
		EXP_ASSERT( 0 );
	}

	// if there is a lview_port range, try to bind within this range
	if( lview_port_min ){
		for(uint16_t cur_port = lview_port_min; cur_port <= lview_port_max; cur_port++){
			ipport_addr_t	listen_ipport(lview_addr, cur_port);
			inet_err_t	inet_err;
			udp_resp_t *	udp_resp;
			// try to start a udp_resp_t on the listen_ipport
			udp_resp	= nipmem_new udp_resp_t();
			inet_err	= udp_resp->start(listen_ipport, NULL, NULL);
			nipmem_zdelete	udp_resp;
			// if the start() failed, goto the next
			if( inet_err.failed() )	continue;
			// if the binding succeed, set the cur_port and leave the loop
			chosen_lport	= cur_port;
			break;	
		}
	}
	
	// if there are no lview_port range or if bind within it failed, bind dynamicaly 
	// and return a null pview
	if( !lview_port_min || chosen_lport == 0 ){
		ipport_addr_t	listen_ipport(lview_addr, 0);
		inet_err_t	inet_err;
		udp_resp_t *	udp_resp;
		// try to start a udp_resp_t on listen_ipport
		udp_resp	= nipmem_new udp_resp_t();
		inet_err	= udp_resp->start(listen_ipport, NULL, NULL);
		DBG_ASSERT( !inet_err.failed() );
		// update the listen_ipport with the bound address
		listen_ipport	= udp_resp->get_listen_addr();
		// delete the just created udp_resp_t
		nipmem_zdelete	udp_resp;
		// log to debug
		if( lview_port_min )	KLOG_INFO("unable to bind within configured lview_port_min/max range, falling back on dynamic allocation.");
		// return the result to the caller with a null pview
		return ipport_aview_t().lview(listen_ipport);
	}


	// sanity check - here, chosen_lport has been allocated within configured lview_port range 
	DBG_ASSERT( chosen_lport >= lview_port_min && chosen_lport <= lview_port_max );
	DBG_ASSERT( lview_port_min && lview_port_max );

	// build the result
	ipport_aview_t	listen_aview;
	// set the listen_aview.lview()
	listen_aview.lview	( ipport_addr_t(lview_addr, chosen_lport) );
	// set the listen_aview.pview() IIF there is a configured pview_port range
	if( pview_port_min ) 	listen_aview.pview( ipport_addr_t(pview_addr, pview_port_min+(chosen_lport-lview_port_min)) );
	// return the just built listen_aview
	return listen_aview;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmit_listen_ipport handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    
/** \brief build a xmit_listen_ipport from a listen_aview and the destination ipport 
 *         to which send this xmit_listen_ipport
 * 
 * - authoritative description on the header of neoip_ipport_aview_helper.cpp
 * 
 * @return the xmit_listen_ipport to send to the dst_ipport
 */
ipport_addr_t	ipport_aview_helper_t::listen_aview2xmit(const ipport_aview_t &listen_aview
						, const ipport_addr_t &dst_ipport)	throw()
{
	// sanity check - the dst_ipport MUST be is_fully_qualified
	DBG_ASSERT( dst_ipport.is_fully_qualified() );

	// sanity check - the listen_aview MUST be is_run_ok()
	DBG_ASSERT( listen_aview.is_run_ok() );

	// if the dst_ipport has a non-public ip address, return the listen_aview.lview()
	// "- if the dest ipaddr is non public, the sent xmit_listen_ipport is listen_aview.lview()
	//  - the dest_ipaddr may be a loopback one (127.0.0.1/8) or a private one (rfc1912)
	//    or a linklocal one(169.254.0.0/16)
	//  - the queried is assumed behind the same NAT realm
	//  - host1 being inetreach_ok or not is irrelevant in this case as they are both
	//    behind the same NAT realm"
	if(!dst_ipport.ipaddr().is_public())	return listen_aview.lview();

	// - NOTE: at this point, the remote_oaddr has a public ip address

	// if the dst_ipport has a public ipaddr, and listen_aview.pview() is not null, return it
	// "- if the dest ipaddr is public, and host1 is inetreach_ok, the sent xmit_listen_ipport
	//    is the listen_aview.pview()"
	if( !listen_aview.pview().is_null() )	return listen_aview.pview();
	
	// in all other cases, return a null ipport_addr_t
	// "- if the dest ipaddr is public, and host1 is NOT inetreach_ok, the sent xmit_listen_ipport
	//    is a null ipport_addr_t"
	return ipport_addr_t();
}

/** \brief build a dest_listen_ipport from a xmit_listen_ipport and the source ipport 
 *         which sent this xmit_listen_ipport.
 * 
 * - authoritative description on the header of neoip_ipport_aview_helper.cpp
 * 
 * @return a ipport_addr_t. IF it is_fully_qualified(), it can be used as a destination
 *         address toward the sender of the xmit_listen_ipport. ELSE it is null() and 
 *         means the sender of the xmit_listen_ipport is unreachable to the local peer. 
 */
ipport_addr_t	ipport_aview_helper_t::listen_xmit2dest(const ipport_addr_t &xmit_listen_ipport
						, const ipport_addr_t &src_ipport)	throw()
{
	// sanity check - src_ipport MUST be is_fully_qualified
	DBG_ASSERT( src_ipport.is_fully_qualified() );

	// - if the recved xmit_listen_ipport is null, the sender declared itself unreachable
	//   by the local host, so return a null ipport_addr_t
	if( xmit_listen_ipport.is_null() )	return ipport_addr_t();

	// sanity check - the xmit_listen_ipport MUST have a port and a non null ip_addr_t
	// - TODO ugly!!! find a better way.. glib has some macro to do stuff like that
	// - maybe a good idea to look at it
	DBGNET_ASSERT( !xmit_listen_ipport.ipaddr().is_null() && xmit_listen_ipport.port() );
	if( !(!xmit_listen_ipport.ipaddr().is_null() && xmit_listen_ipport.port()) )
		return ipport_addr_t();

	// - "if the xmit_listen_ipport.ipaddr() is_fully_qualified(), return xmit_listen_ipport itself"
	if( xmit_listen_ipport.ipaddr().is_fully_qualified() )	return xmit_listen_ipport;

	// - "if the xmit_listen_ipport.ipaddr() is_any(), remplace it by the src_ipport.ipaddr()
	//    in xmit_listen_ipport and return the resulting ipport_addr_t."
	if( xmit_listen_ipport.ipaddr().is_any() ){
		ipport_addr_t dest_ipport = xmit_listen_ipport;
		dest_ipport.ipaddr	(src_ipport.ipaddr());
		DBG_ASSERT( dest_ipport.is_fully_qualified() );
		return dest_ipport;
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0);
	return ipport_addr_t();
}

NEOIP_NAMESPACE_END;






