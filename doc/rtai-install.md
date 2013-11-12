RTAI Installation Guide
=======================


by **Fabián C. Tommasini** (adapted from [Monteiro][1] and [Ari][2]).


Software
--------

- GNU/Linux distro: Ubuntu Server 12.04 LTS (64 bits)
- C Compiler: GCC 4.6.3
- Linux kernel 3.4.58
- RTAI 3.9 from Git repository @ GitHub.com - changeset: af1af992ca1b830b881bc359db7e7499b6956612


Step 0: Pre-requisites
----------------------

It is necessary to have installed all the packages associated with kernel compilation and Git CVS:

	$ sudo aptitude install build-essential kernel-package ncurses-dev git-core


Step 1: Preparation
-------------------

Go to home directory:

	$ cd ~

Download Linux kernel 3.4.58 from [kernel.org](http://www.kernel.org):

	$ wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.4.58.tar.xz

Download RTAI 3.9 from [Git repository](https://github.com/ShabbyX/RTAI):

	$ git clone https://github.com/ShabbyX/RTAI.git

Check out specific changeset (this is not absolutely necesary):

	$ git checkout af1af992ca1b830b881bc359db7e7499b6956612

> *Note*: For quite awhile, all the commands are run as root until you reach near the bottom so feel free to just run:

> `$ sudo -s`

Put this file into `/usr/src/`:

	# cp linux-3.4.58.tar.xz /usr/src/

Unpack the file:

	# cd /usr/src/
	# tar Jxvf linux-3.4.58.tar.xz

Delete the tarballs:

	# rm linux-3.4.58.tar.xz

Rename linux source directory to a more descriptive name (optional):

	# mv linux-3.4.58 linux-3.4.58-rtai

Create symbolic links to the new folder:

	# ln -snf linux-3.4.58-rtai linux


Step 3: Kernel Patching and Configuration
-----------------------------------------

Patch the kernel source with the correspondent RTAI patch:

	# cd /usr/src/linux
	# make mrproper
	# patch -p1 < ~/RTAI/base/arch/x86/patches/hal-linux-3.4.58-x86-1.patch

> *Note*: x86_64 and i386 have been merged into x86 arch (that happened around 2.6.24 iirc kernel version). Use x86 if you want 32- or 64-bit kernels.

In order to make the new kernel configuration the most similar to the already
installed one you can do the two next steps:

	# cp /boot/config-`uname -r` ./.config
	# make oldconfig

Then press Enter to all the prompts.
Now is time to configure the new kernel:

	# make menuconfig

### Kernel configuration

To determining your basic hardware, open up a new tab in the terminal or a new
terminal window and run the following command as root:

	# lspci -k | grep "driver in use"

In the configuration menu you should do:

- Processor type and features > Interrupt pipeline = yes
- Processor type and features > Preemption model = Preemptible kernel (Low-latency Desktop)

Because power management is a "latency killer" you should turn it off:

- Power management options > Suspend to RAM and standby = no
- Power management options > Hibernation (aka 'suspend to disk') = no
- Power management options > Power management debug support = no
- Power management options > CPU Frequency scaling > CPU Frequency scaling = no

Enable network support for your ethernet controller:

- Device drivers > Network device support > Ethernet driver support = (choose yours)

Use the arrow keys on your keyboard, and enabling it by pressing "Y" on the highlighted option to compile the driver into the kernel.

Enable SATA / PATA / SCSI / Hard drive support for your hardware:

- Device drivers > Serial ATA and Parallel ATA drivers = (choose yours)

> *Note*: If you specifically use AHCI for your Solid State Drive (SSD)
or Hard Disk Drive (HDD) then you do not need to enable ATA SFF support.
ATA SFF support simply shows more possible drivers. It is recommended to
turn on support for any and ALL drive controllers mentioned in the `lspci -k` command.

To have a new kernel with a understable name you should choose a good suffix to
it, i.e. "`-rtai`" (without quotes):

- General setup > Local version - append to kernel release = -rtai

If you know your machine processor type you can choose the right one:

- Processor type and features > Processor family = (choose yours)

It is possible use multiple cores on multicore machines:

- Processor type and features > Multi core scheduling = no/yes

After all changes exit and say **YES** to save the configuration.

> *Note*: More options can be reading in [Monteiro][1] and [Ari][2].

Step 4: Kernel Compilation
--------------------------

Compiling and installing the kernel:

> *Tip*: The following `make` command automatically detects the number of CPU cores and compiles the kernel with however many jobs that number might be. This speeds up compiling times on SMP systems.

	# make -j$(cat /proc/cpuinfo | grep processor | wc -l)
	# make modules_install
	# cp -prvL arch/x86/boot/bzImage /boot/vmlinuz-3.4.58-rtai
	# cp -prvL System.map /boot/System.map-3.4.58-rtai
	# cp -prvL .config /boot/config-3.4.58-rtai

Update your boot loader:

	# update-grub

Now you can reboot into your new kernel. Optionally you can manually setting the new kernel as the default.

### Manually setting the patched kernel as the default

To manually set a specific kernel to boot, you must edit the `/etc/default/grub` file as root:

	# nano /etc/default/grub

The line to edit is the `GRUB_DEFAULT=0`.<br />
Change this line to (for more details you can see [Ubuntu Community Documentation][3]):

	GRUB_DEFAULT="Previous Linux versions>Ubuntu, with Linux 3.4.58-rtai"

Then save the file and update the GRUB 2 configuration file using the following command:

	# update-grub

Reboot.


Step 5: Configuring and Compiling RTAI
--------------------------------------

If the above steps are completed, boot up your new kernel.
Now, enter to the RTAI folder:

	$ cd ~/RTAI

Now lets conﬁgure the RTAI:

	$ ./autogen.sh
	$ make menuconfig

The following options should be veriﬁed in the ncurses menu that will show up:

- General > Installation directory = (leave the default as /usr/realtime)
- General > Linux Build Tree = (the path to the conﬁgured kernel /usr/src/linux)
- Machine (x86_64) > Number of CPUs (SMP-only) = (number of CPUs/cores)

All set, now just exit and reply **YES** to save the conﬁguration.
Compile RTAI:

	$ make

Now, install RTAI (default: `/usr/realtime`):

	$ sudo make install

or (as root):

	# make install

Now reboot your computer and boot our new kernel with RTAI installed.


Step 6: Testing RTAI
--------------------

It’s time to ﬁnally test the installation. For this, do the following:

	$ cd /usr/realtime/testsuite/kern/latency
	$ sudo ./run


Step 7: Post-installation
-------------------------

Add the `/usr/realtime` directory to the system path:
Edit the `~/.bashrc` file and add the following line to the end.

	export PATH=${PATH}:/usr/realtime

Then reload the configuration:

	$ . ~/.bashrc


Appendix: Uninstalling Patched Kernel
-------------------------------------

(Extracted from [Ubuntu Forums][4])

	# rm /boot/vmlinuz-3.4.58-rtai
	# rm /boot/System.map-3.4.58-rtai
	# rm /boot/config-3.4.58-rtai
	# rm -r /lib/modules/3.4.58-rtai/
	# update-grub


[1]: https://www.rtai.org/RTAICONTRIB/RTAI_Installation_Guide.pdf	"Joao Monteiro. RTAI Installation Complete Guide."

[2]: https://github.com/ShabbyX/RTAI/blob/master/README.INSTALL	"Alec Ari. README.INSTALL from RTAI GitHub Repository."

[3]: https://help.ubuntu.com/community/Grub2/Submenus	"Ubuntu Community Documentation. Grub2/Submenus."

[4]: http://ubuntuforums.org/showthread.php?t=1278547	"Ubuntu Forums. Uninstall custom kernel."
