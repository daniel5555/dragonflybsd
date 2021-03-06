/*
 *
 * ===================================
 * HARP  |  Host ATM Research Platform
 * ===================================
 *
 *
 * This Host ATM Research Platform ("HARP") file (the "Software") is
 * made available by Network Computing Services, Inc. ("NetworkCS")
 * "AS IS".  NetworkCS does not provide maintenance, improvements or
 * support of any kind.
 *
 * NETWORKCS MAKES NO WARRANTIES OR REPRESENTATIONS, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE, AS TO ANY ELEMENT OF THE
 * SOFTWARE OR ANY SUPPORT PROVIDED IN CONNECTION WITH THIS SOFTWARE.
 * In no event shall NetworkCS be responsible for any damages, including
 * but not limited to consequential damages, arising from or relating to
 * any use of the Software or related support.
 *
 * Copyright 1994-1998 Network Computing Services, Inc.
 *
 * Copies of this Software may be made, however, the above copyright
 * notice must be reproduced on all copies.
 *
 *	@(#) $FreeBSD: src/sbin/atm/atm/atm.h,v 1.4 1999/08/28 00:12:17 peter Exp $
 *	@(#) $DragonFly: src/sbin/atm/atm/atm.h,v 1.5 2006/10/16 00:15:35 pavalos Exp $
 *
 */

/*
 * User configuration and display program
 * --------------------------------------
 *
 * Control blocks
 *
 */

#define	MAX_NIFS	256		/* Max network interfaces */
#define	MIN_VCI		32		/* Smallest non-reserved VCI */

#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

 
/*
 * User commands
 */
struct cmd {
	const char	*name;		/* Command name */
	int	minp;			/* Minimum number of parameters */
	int	maxp;			/* Maximum number of parameters */
	void	(*func)			/* Processing function */
		(int, char **, const struct cmd *);
	const char	*help;		/* User help string */
};


/*
 * Supported signalling protocols
 */
struct proto {
	const char	*p_name;	/* Protocol name */
	u_char	p_id;			/* Protocol id */ 
};


/*
 * Table of state names
 */
struct state {
	const char	*s_name;	/* State name */
	u_char	s_id;			/* State id */ 
};


/*
 * Supported signalling protocol states
 */
struct proto_state {
	const char 	*p_name;	/* Signalling manager name */
	const struct state *p_state;	/* Protocol state table */
	const struct state *v_state;	/* Protocol VCC state table */
	u_char		p_id;		/* Protocol ID */ 
};


/*
 * Supported VCC owners
 */
struct owner {
	const char	*o_name;	/* Owner name */
	u_int	o_sap;			/* Owner's SAP */
	void	(*o_pvcadd)		/* PVC ADD processing function */
			(int, char **, const struct cmd *, struct atmaddreq *,
			     struct air_int_rsp *);
};


/*
 * Supported AALs
 */
struct aal {
	const char	*a_name;	/* AAL name */
	u_char	a_id;			/* AAL code */ 
};


/*
 * Supported encapsulations
 */
struct encaps {
	const char	*e_name;	/* Encapsulation name */
	u_char	e_id;			/* Encapsulation code */ 
};


/*
 * External variables
 */
extern char	*prog;			/* Program invocation */
extern char	prefix[];		/* Current command prefix */

/*
 * Global function declarations
 */

	/* atm_eni.c */
void		show_eni_stats(char *, int, char **);

	/* atm_fore200.c */
void		show_fore200_stats(char *, int, char **);

	/* atm_inet.c */
void		ip_pvcadd (int, char **, const struct cmd *, struct atmaddreq *,
			struct air_int_rsp *);

	/* atm_print.c */
void		print_arp_info(struct air_arp_rsp *);
void		print_asrv_info(struct air_asrv_rsp *);
void		print_cfg_info(struct air_cfg_rsp *);
void		print_intf_info(struct air_int_rsp *);
void		print_ip_vcc_info(struct air_ip_vcc_rsp *);
void		print_netif_info(struct air_netif_rsp *);
void		print_intf_stats(struct air_phy_stat_rsp *);
void		print_vcc_stats(struct air_vcc_rsp *);
void		print_vcc_info(struct air_vcc_rsp *);
void		print_version_info(struct air_version_rsp *);

	/* atm_set.c */
void		set_arpserver(int, char **, const struct cmd *);
void		set_macaddr(int, char **, const struct cmd *);
void		set_netif(int, char **, const struct cmd *);
void		set_prefix(int, char **, const struct cmd *);

	/* atm_show.c */
void		show_arp(int, char **, const struct cmd *);
void		show_arpserv(int, char **, const struct cmd *);
void		show_config(int, char **, const struct cmd *);
void		show_intf(int, char **, const struct cmd *);
void		show_ip_vcc(int, char **, const struct cmd *);
void		show_netif(int, char **, const struct cmd *);
void		show_intf_stats(int, char **, const struct cmd *);
void		show_vcc_stats(int, char **, const struct cmd *);
void		show_vcc(int, char **, const struct cmd *);
void		show_version(int, char **, const struct cmd *);

	/* atm_subr.c */
const char *	get_vendor(int);
const char *	get_adapter(int);
const char *	get_media_type(int);
const char *	get_bus_type(int);
const char *	get_adapter_name(char *);
const char *	format_mac_addr(Mac_addr *);
int		parse_ip_prefix(char *, struct in_addr *);
int		compress_prefix_list(struct in_addr *, int);
void		check_netif_name(char *);
void		sock_error(int);
