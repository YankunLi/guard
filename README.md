# guard - Destributed storage system Ceph monitor
guard is a monitoring and debugging tool to capture ceph related statistics
and prepare them visually in a human friendly way.

## Requirement

Prepare a Ceph cluster environment, global configuration file ceph.conf and
user client.admin kering of Ceph cluster. Put ceph.conf and client.admin
keyring in /etc/ceph/ directory.


## Centos (6) Installation

```
git clone https://github.com/YankunLi/guard.git
cd guard
yum install ncurses-devel librados2-devel librados2
make
make install
```
--------------
### Usage

To run guard in the default curses mode:

> guard
