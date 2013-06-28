/*
 * Copyright (c) 2011-2012 The DragonFly Project.  All rights reserved.
 *
 * This code is derived from software contributed to The DragonFly Project
 * by Matthew Dillon <dillon@dragonflybsd.org>
 * by Venkatesh Srinivas <vsrinivas@dragonflybsd.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of The DragonFly Project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific, prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "hammer2.h"

static void usage(int code);

int DebugOpt;
int VerboseOpt;
int QuietOpt;
int NormalExit = 1;	/* if set to 0 main() has to pthread_exit() */

void
set_comp_mode_recursive(char *directory, int comp_method)
{
	DIR *dir;
	if ((dir = opendir (directory)) == NULL) {
        fprintf(stderr, "ERROR while trying to set the mode recursively: %s\n", strerror(errno));
		exit(3);
    }
    struct dirent *dent;
    int lenght = strlen(directory);
    char name[HAMMER2_INODE_MAXNAME];
    strcpy(name, directory);
    name[lenght] = "/";
    ++lenght;
    errno = 0;
    dent = readdir(dir);
    while (dent != NULL) {
		if ((strcmp(dent->d_name, ".") != 0) &&
		 (strcmp(dint->d_name, "..") != 0)) {
			strncpy(name + lenght, dent->d_name, HAMMER2_INODE_MAXNAME - lenght);
			int fd = hammer2_ioctl_handle(name);
			//printf("got inode with fd = %d\n", fd);
			hammer2_ioc_inode_t inode;
			int res = ioctl(fd, HAMMER2IOC_INODE_GET, &inode);
			if (res < 0) {
				fprintf(stderr, "ERROR during recursion: %s\n", strerror(errno));
				exit(3);
			}
			if (inode.ip_data.type == HAMMER2_OBJTYPE_DIRECTORY) {
				set_comp_mode_recursive(name, comp_method);
			}
			inode.ip_data.comp_algo = comp_method;
			res = ioctl(fd, HAMMER2IOC_INODE_SET, &inode);
			if (res < 0) {
				fprintf(stderr, "ERROR during recursion after trying to set the mode: %s\n", strerror(errno));
				exit(3);
			}
		}
		dent = readdir(dir);
	}
	if (errno != 0) {
		fprintf(stderr, "ERROR during itertione: %s\n", strerror(errno));
		exit(3);
    }
}

int
main(int ac, char **av)
{
	const char *sel_path = NULL;
	const char *uuid_str = NULL;
	const char *arg;
	int pfs_type = HAMMER2_PFSTYPE_NONE;
	int all_opt = 0;
	int ecode = 0;
	int ch;

	srandomdev();
	signal(SIGPIPE, SIG_IGN);
	dmsg_crypto_setup();

	/*
	 * Core options
	 */
	while ((ch = getopt(ac, av, "adqs:t:u:v")) != -1) {
		switch(ch) {
		case 'a':
			all_opt = 1;
			break;
		case 'd':
			DebugOpt = 1;
			break;
		case 's':
			sel_path = optarg;
			break;
		case 't':
			/*
			 * set node type for mkpfs
			 */
			if (strcasecmp(optarg, "ADMIN") == 0) {
				pfs_type = HAMMER2_PFSTYPE_ADMIN;
			} else if (strcasecmp(optarg, "CACHE") == 0) {
				pfs_type = HAMMER2_PFSTYPE_CACHE;
			} else if (strcasecmp(optarg, "COPY") == 0) {
				pfs_type = HAMMER2_PFSTYPE_COPY;
			} else if (strcasecmp(optarg, "SLAVE") == 0) {
				pfs_type = HAMMER2_PFSTYPE_SLAVE;
			} else if (strcasecmp(optarg, "SOFT_SLAVE") == 0) {
				pfs_type = HAMMER2_PFSTYPE_SOFT_SLAVE;
			} else if (strcasecmp(optarg, "SOFT_MASTER") == 0) {
				pfs_type = HAMMER2_PFSTYPE_SOFT_MASTER;
			} else if (strcasecmp(optarg, "MASTER") == 0) {
				pfs_type = HAMMER2_PFSTYPE_MASTER;
			} else {
				fprintf(stderr, "-t: Unrecognized node type\n");
				usage(1);
			}
			break;
		case 'u':
			/*
			 * set uuid for mkpfs, else one will be generated
			 * (required for all except the MASTER node_type)
			 */
			uuid_str = optarg;
			break;
		case 'v':
			if (QuietOpt)
				--QuietOpt;
			else
				++VerboseOpt;
			break;
		case 'q':
			if (VerboseOpt)
				--VerboseOpt;
			else
				++QuietOpt;
			break;
		default:
			fprintf(stderr, "Unknown option: %c\n", ch);
			usage(1);
			/* not reached */
			break;
		}
	}

	/*
	 * Adjust, then process the command
	 */
	ac -= optind;
	av += optind;
	if (ac < 1) {
		fprintf(stderr, "Missing command\n");
		usage(1);
		/* not reached */
	}

	if (strcmp(av[0], "connect") == 0) {
		/*
		 * Add cluster connection
		 */
		if (ac < 2) {
			fprintf(stderr, "connect: missing argument\n");
			usage(1);
		}
		ecode = cmd_remote_connect(sel_path, av[1]);
	} else if (strcmp(av[0], "debugspan") == 0) {
		/*
		 * Debug connection to the target hammer2 service and run
		 * the CONN/SPAN protocol.
		 */
		if (ac < 2) {
			fprintf(stderr, "debugspan: requires hostname\n");
			usage(1);
		}
		ecode = cmd_debugspan(av[1]);
	} else if (strcmp(av[0], "disconnect") == 0) {
		/*
		 * Remove cluster connection
		 */
		if (ac < 2) {
			fprintf(stderr, "disconnect: missing argument\n");
			usage(1);
		}
		ecode = cmd_remote_disconnect(sel_path, av[1]);
	} else if (strcmp(av[0], "hash") == 0) {
		ecode = cmd_hash(ac - 1, (const char **)(void *)&av[1]);
	} else if (strcmp(av[0], "status") == 0) {
		/*
		 * Get status of PFS and its connections (-a for all PFSs)
		 */
		ecode = cmd_remote_status(sel_path, all_opt);
	} else if (strcmp(av[0], "pfs-clid") == 0) {
		/*
		 * Print cluster id (uuid) for specific PFS
		 */
		if (ac < 2) {
			fprintf(stderr, "pfs-clid: requires name\n");
			usage(1);
		}
		ecode = cmd_pfs_getid(sel_path, av[1], 0);
	} else if (strcmp(av[0], "pfs-fsid") == 0) {
		/*
		 * Print private id (uuid) for specific PFS
		 */
		if (ac < 2) {
			fprintf(stderr, "pfs-fsid: requires name\n");
			usage(1);
		}
		ecode = cmd_pfs_getid(sel_path, av[1], 1);
	} else if (strcmp(av[0], "pfs-list") == 0) {
		/*
		 * List all PFSs
		 */
		ecode = cmd_pfs_list(sel_path);
	} else if (strcmp(av[0], "pfs-create") == 0) {
		/*
		 * Create new PFS using pfs_type
		 */
		if (ac < 2) {
			fprintf(stderr, "pfs-create: requires name\n");
			usage(1);
		}
		ecode = cmd_pfs_create(sel_path, av[1], pfs_type, uuid_str);
	} else if (strcmp(av[0], "pfs-delete") == 0) {
		/*
		 * Delete a PFS by name
		 */
		if (ac < 2) {
			fprintf(stderr, "pfs-delete: requires name\n");
			usage(1);
		}
		ecode = cmd_pfs_delete(sel_path, av[1]);
	} else if (strcmp(av[0], "snapshot") == 0) {
		/*
		 * Create snapshot with optional pfs-type and optional
		 * label override.
		 */
		if (ac > 2) {
			fprintf(stderr, "pfs-snapshot: too many arguments\n");
			usage(1);
		}
		if (ac != 2)
			ecode = cmd_pfs_snapshot(sel_path, NULL);
		else
			ecode = cmd_pfs_snapshot(sel_path, av[1]);
	} else if (strcmp(av[0], "service") == 0) {
		/*
		 * Start the service daemon.  This daemon accepts
		 * connections from local and remote clients, handles
		 * the security handshake, and manages the core messaging
		 * protocol.
		 */
		ecode = cmd_service();
	} else if (strcmp(av[0], "stat") == 0) {
		ecode = cmd_stat(ac - 1, (const char **)(void *)&av[1]);
	} else if (strcmp(av[0], "leaf") == 0) {
		/*
		 * Start the management daemon for a specific PFS.
		 *
		 * This will typically connect to the local master node
		 * daemon, register the PFS, and then pass its side of
		 * the socket descriptor to the kernel HAMMER2 VFS via an
		 * ioctl().  The process and/or thread context remains in the
		 * kernel until the PFS is unmounted or the connection is
		 * lost, then returns from the ioctl.
		 *
		 * It is possible to connect directly to a remote master node
		 * instead of the local master node in situations where
		 * encryption is not desired or no local master node is
		 * desired.  This is not recommended because it represents
		 * a single point of failure for the PFS's communications.
		 *
		 * Direct kernel<->kernel communication between HAMMER2 VFSs
		 * is theoretically possible for directly-connected
		 * registrations (i.e. where the spanning tree is degenerate),
		 * but not recommended.  We specifically try to reduce the
		 * complexity of the HAMMER2 VFS kernel code.
		 */
		ecode = cmd_leaf(sel_path);
	} else if (strcmp(av[0], "shell") == 0) {
		/*
		 * Connect to the command line monitor in the hammer2 master
		 * node for the machine using HAMMER2_DBG_SHELL messages.
		 */
		ecode = cmd_shell((ac < 2) ? NULL : av[1]);
	} else if (strcmp(av[0], "rsainit") == 0) {
		/*
		 * Initialize a RSA keypair.  If no target directory is
		 * specified we default to "/etc/hammer2".
		 */
		arg = (ac < 2) ? HAMMER2_DEFAULT_DIR : av[1];
		ecode = cmd_rsainit(arg);
	} else if (strcmp(av[0], "rsaenc") == 0) {
		/*
		 * Encrypt the input symmetrically by running it through
		 * the specified public and/or private key files.
		 *
		 * If no key files are specified data is encoded using
		 * "/etc/hammer2/rsa.pub".
		 *
		 * WARNING: no padding is added, data stream must contain
		 *	    random padding for this to be secure.
		 *
		 * Used for debugging only
		 */
		if (ac == 1) {
			const char *rsapath = HAMMER2_DEFAULT_DIR "/rsa.pub";
			ecode = cmd_rsaenc(&rsapath, 1);
		} else {
			ecode = cmd_rsaenc((const char **)(void *)&av[1],
					   ac - 1);
		}
	} else if (strcmp(av[0], "rsadec") == 0) {
		/*
		 * Decrypt the input symmetrically by running it through
		 * the specified public and/or private key files.
		 *
		 * If no key files are specified data is decoded using
		 * "/etc/hammer2/rsa.prv".
		 *
		 * WARNING: no padding is added, data stream must contain
		 *	    random padding for this to be secure.
		 *
		 * Used for debugging only
		 */
		if (ac == 1) {
			const char *rsapath = HAMMER2_DEFAULT_DIR "/rsa.prv";
			ecode = cmd_rsadec(&rsapath, 1);
		} else {
			ecode = cmd_rsadec((const char **)(void *)&av[1],
					   ac - 1);
		}
	} else if (strcmp(av[0], "show") == 0) {
		/*
		 * Raw dump of filesystem.  Use -v to check all crc's, and
		 * -vv to dump bulk file data.
		 */
		if (ac != 2) {
			fprintf(stderr, "show: requires device path\n");
			usage(1);
		} else {
			cmd_show(av[1], 0);
		}
	} else if (strcmp(av[0], "freemap") == 0) {
		/*
		 * Raw dump of freemap.  Use -v to check all crc's, and
		 * -vv to dump bulk file data.
		 */
		if (ac != 2) {
			fprintf(stderr, "freemap: requires device path\n");
			usage(1);
		} else {
			cmd_show(av[1], 1);
		}
	} else if (strcmp(av[0], "setcmp") == 0) {
		if (ac < 3 || ac > 4) {
			fprintf(stderr, "setcmp: requires compression method and directory/file path\n");
			usage(1);
		} else {
			if (ac == 3) {
				int comp_method;
				if (strcmp(av[1], "0") == 0) {
					printf("Will turn off compression on directory/file %s\n", av[2]);
					comp_method = HAMMER2_COMP_NONE;
				} else if (strcmp(av[1], "1") == 0) {
					printf("Will set zero-checking compression on directory/file %s.\n", av[2]);
					comp_method = HAMMER2_COMP_AUTOZERO;
				} else if (strcmp(av[1], "2") == 0) {
					printf("Will set LZ4 compression on directory/file %s.\n", av[2]);
					comp_method = HAMMER2_COMP_LZ4;
				} else {
					printf("Unknown compression method.\n");
					exit(1);
				}
				int fd = hammer2_ioctl_handle(av[2]);
				printf("got inode with fd = %d\n", fd);
				hammer2_ioc_inode_t inode;
				int res = ioctl(fd, HAMMER2IOC_INODE_GET, &inode);
				if (res < 0) {
					fprintf(stderr, "ERROR before setting the mode: %s\n", strerror(errno));
					exit(3);
				}
				inode.ip_data.comp_algo = comp_method;
				res = ioctl(fd, HAMMER2IOC_INODE_SET, &inode);
				if (res < 0) {
					fprintf(stderr, "ERROR after trying to set the mode: %s\n", strerror(errno));
					//exit(3);
				}
			}
			else {
				if (strcmp(av[1], "-r") == 0) {
					int comp_method;
					if (strcmp(av[2], "0") == 0) {
						printf("Will turn off compression on directory/file %s\n", av[3]);
						comp_method = HAMMER2_COMP_NONE;
					} else if (strcmp(av[2], "1") == 0) {
						printf("Will set zero-checking compression on directory/file %s.\n", av[3]);
						comp_method = HAMMER2_COMP_AUTOZERO;
					} else if (strcmp(av[2], "2") == 0) {
						printf("Will set LZ4 compression on directory/file %s.\n", av[3]);
						comp_method = HAMMER2_COMP_LZ4;
					} else {
						printf("Unknown compression method.\n");
						exit(1);
					}
					int fd = hammer2_ioctl_handle(av[2]);
					printf("got inode with fd = %d\n", fd);
					hammer2_ioc_inode_t inode;
					int res = ioctl(fd, HAMMER2IOC_INODE_GET, &inode);
					if (res < 0) {
						fprintf(stderr, "ERROR before setting the mode: %s\n", strerror(errno));
						exit(3);
					}
					if (inode.ip_data.type != HAMMER2_OBJTYPE_DIRECTORY) {
						printf("setcmp: the specified object is not a directory, nothing changed.\n");
						exit(1);
					}
					printf("Attention: recursive compression mode setting demanded, this may take a while...\n");
					set_comp_mode_recursive(av[3], comp_method);
					inode.ip_data.comp_algo = comp_method;
					res = ioctl(fd, HAMMER2IOC_INODE_SET, &inode);
					if (res < 0) {
						fprintf(stderr, "ERROR after trying to set the mode: %s\n", strerror(errno));
						//exit(3);
					}
				}
				else {
					printf("setcmp: Unrecognized option.\n");
					exit(1);
				}	
			}
			printf("Compression mode set.\n");			
			/* Do something here. */
		}
	} else if (strcmp(av[0], "printinode") == 0) {
		if (ac != 2) {
			fprintf(stderr, "printinode: requires directory/file path\n");
			usage(1);
		} else {
			printf("Printing the inode's contents of directory/file %s\n", av[1]);
			int fd = hammer2_ioctl_handle(av[1]);
			if (fd != -1) {
				printf("got inode with fd = %d\n", fd);
				hammer2_ioc_inode_t inode;
				int res = ioctl(fd, HAMMER2IOC_INODE_GET, &inode);
				hammer2_inode_data_t inode_data;
				inode_data = inode.ip_data;
				printf("Got res = %d\n", res);
				printf("Printing inode data.\n");
				/*printf("version = %d\n", inode_data.version);
				printf("uflags = %d\n", inode_data.uflags);
				printf("rmajor = %d\n", inode_data.rmajor);
				printf("rminor = %d\n", inode_data.rminor);
				printf("ctime = %u !\n", (unsigned int)inode_data.ctime);
				printf("mtime = %u !\n", (unsigned int)inode_data.mtime);*/
				printf("type = %d\n", inode_data.type);
				printf("op_flags = %d\n", inode_data.op_flags);
				/*printf("cap_flags = %d\n", inode_data.cap_flags);
				printf("mode = %d\n", inode_data.mode);
				printf("inum = %u !\n", (unsigned int)inode_data.inum);
				printf("size = %u !\n", (unsigned int)inode_data.size),*/
				printf("name_key = %u !\n", (unsigned int)inode_data.name_key);
				/*printf("name_len = %d\n", inode_data.name_len);
				printf("ncopies = %d\n", inode_data.ncopies);*/
				printf("comp_algo = %d\n", inode_data.comp_algo);
				if (inode_data.op_flags != HAMMER2_OPFLAG_DIRECTDATA) {
					int i;
					for (i = 0; i < HAMMER2_SET_COUNT; ++i) {
						if (inode_data.u.blockset.blockref[i].type != HAMMER2_BREF_TYPE_EMPTY) {
							printf("blockrefs %d type = %d\n", i, inode_data.u.blockset.blockref[i].type);
							printf("blockrefs %d methods = %d\n", i, inode_data.u.blockset.blockref[i].methods);
							printf("blockrefs %d copyid = %d\n", i, inode_data.u.blockset.blockref[i].copyid);
							printf("blockrefs %d flags = %d\n", i, inode_data.u.blockset.blockref[i].flags);
							printf("blockrefs %d key = %u !\n", i, (unsigned int)inode_data.u.blockset.blockref[i].key);
						}
						else
							printf("blockrefs %d is empty.\n", i);
					}
				}
				else {
					printf("This inode has data instead of blockrefs.\n");
				}
			}
			else {
				exit(0);
			}
			/* Do something here. */
		}
	} else {
		fprintf(stderr, "Unrecognized command: %s\n", av[0]);
		usage(1);
	}

	/*
	 * In DebugMode we may wind up starting several pthreads in the
	 * original process, in which case we have to let them run and
	 * not actually exit.
	 */
	if (NormalExit) {
		return (ecode);
	} else {
		pthread_exit(NULL);
		_exit(2);	/* NOT REACHED */
	}
}

static
void
usage(int code)
{
	fprintf(stderr,
		"hammer2 [-s path] command...\n"
		"    -s path            Select filesystem\n"
		"    -t type            PFS type for pfs-create\n"
		"    -u uuid            uuid for pfs-create\n"
		"\n"
		"    connect <target>   Add cluster link\n"
		"    disconnect <target> Del cluster link\n"
		"    hash filename*     Print directory hash\n"
		"    status             Report cluster status\n"
		"    pfs-list           List PFSs\n"
		"    pfs-clid <label>   Print cluster id for specific PFS\n"
		"    pfs-fsid <label>   Print private id for specific PFS\n"
		"    pfs-create <label> Create a PFS\n"
		"    pfs-delete <label> Destroy a PFS\n"
		"    snapshot           Snapshot a PFS\n"
		"    service            Start service daemon\n"
		"    stat [<path>]	Return inode quota & config\n"
		"    leaf               Start pfs leaf daemon\n"
		"    shell [<host>]     Connect to debug shell\n"
		"    debugspan <target> Connect to target, run CONN/SPAN\n"
		"    rsainit            Initialize rsa fields\n"
		"    show devpath       Raw hammer2 media dump\n"
		"    freemap devpath    Raw hammer2 media dump\n"
		"    setcmp directory   Sets compression mode on a directory\n"
	);
	exit(code);
}
