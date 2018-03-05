# Vagrant

## 1 基本操作
-

### 1.1 添加box
-

[ref](https://ninghao.net/blog/2077)

vagrant box add centos-6.5 centos-6.5.box --provider=vmware_fusion --name=dev # vagrant box add 名称 路径 provider(vmware_fusion/virtualbox) name  
vagrant box add centos7.4 centos7.4-cgdb-zsh-20171123.box --provider=virtualbox --name=centos74  
vagrant box list # 列出现有的box   
vagrant box remove NAME # 删除指定虚拟机box   
vagrant box outdated # 升级box   

Vagrant 会把 Box 文件复制到系统的某个特定的目录($VAGRANT_HOME)的下面，完成以后，你就可以把桌面上的 Box 文件删除掉了。   
rm centos-6.5.box

[vagrant官方box](http://www.vagrantbox.es)

### 1.2 运行box
-

先找个地方去创建一个目录，这个目录就是你的项目所在的目录，它会自动跟虚拟机上的某个目录同步，也就是在你电脑上的这个目录里面的文件，你同样可以在虚拟机里的某个目录里面找到。

文件映射关系可以从vagrant up的时候如下提示看到：
/vagrant => /Users/alex/test/vagrant/hadoop-cluster

注意：创建出来的虚拟机目录（/Users/alex/VirtualBox VMs/hadoop-cluster_default_1500891337123_28101）和项目目录（/Users/alex/test/vagrant/hadoop-cluster）不在同一个目录下面。

### 1.3 启动期间加载脚本
-

[ref](http://note.axiaoxin.com/contents/vagrant.html)

在Vagrantfile同级目录新建shell脚本bootstrap.sh:

	#! /usr/bin/env bash
	# 无须sudo
	apt-get install -y git

然后添加Vagrantfile中的provision配置：

	config.vm.provision “shell”, path: “bootstrap.sh”  # 运行shell脚本
	config.vm.provision “shell”, inline: “echo hello”  # 直接运行shell命令

然后执行 vagrant up —provision or vagrant reload —provision

### 1.4 构造box
-

* 先删除虚机下面这个文件，否则启动新的配置好网络的虚拟机时候会遇到一些问题。

	/etc/udev/rules.d/70-persistent-net.rules

* vagrant package  

一个项目多主机情况下，系统会提示:

	This command requires a specific VM name to target in a multi-VM environment.   
 vagrant package node1 --output node1.box

* vagrant box add node1 node1.box

把node.box添加到$VAGRANT_HOME/boxes里面后，node1.box就没用了，可以删除也可以发布出去

## 2 常用命令及设置
-

vagrant init centos-6.5 # 初始化一下项目,你会看到一个 Vagrantfile 文件，这个文件是配置当前项目的虚拟机用的.  
vagrant up # 启动虚拟机.  
vagrant up --provider=vmware_fusion #  指定虚拟机的provider.  
vagrant status # 查看一下虚拟机的状态, 如果返回的是 running ，说明虚拟机启动成功  
vagrant global-status   
vagrant global-status --prune  # 如果感觉status数据不正确，可以使用这个进行清空  
vagrant ssh # 连接到虚拟机  
vagrant ssh-config # 输出用于ssh连接的一些信息  
vagrant suspend # 暂停  
vagrant resume # 重新恢复起动  
vagrant halt # 彻底关掉虚拟机的运行  
vagrant reload # 重启  
vagrant destroy # 销毁虚拟机。不打算再用的虚拟机，可以把它删除掉，直接删除项目的目录是不行的，因为为项目指定的虚拟机并不包含在这个项目的目录下面，需要使用命令去删除虚拟机  
vagrant provision # 把虚拟机运行需要的必备软件集预先装好  

VAGRANT_DEFAULT_PROVIDER用于设置默认provider，默认是virtualbox，可以设置为vmware_fusion。
关于virtual box provider的配置请参考 https://www.vagrantup.com/docs/virtualbox/configuration.html
关于docker provider的配置可参考 https://www.vagrantup.com/docs/docker/configuration.html


## 3 配置虚拟机网络
-

### 3.1 forwarded_port 端口转发
-

[ref](https://ninghao.net/blog/2079)

就是你可以设置主机与虚拟机之间的端口的映射关系，这样请求主机上的这个端口，就会把请求转发到虚拟机上的对应的端口上。

config.vm.network "forwarded_port", guest: 80, host: 8080 # 想在访问主机 8080 端口时候，转发到虚拟机上的 80 端口

vagrant port # 可以看到网络映射情况

测试：

在虚拟机运行：sudo python -m SimpleHTTPServer 80

在host打开浏览器访问 http://192.168.0.200:8080 

### 3.2 public_network  公有网络
-

给虚拟机单独配置一个局域网独立ip，让guest与host逻辑上是两台不同的机器，所以在vagrantfile的注释里面可以看到其实是一个bridged network。

config.vm.network "public_network"

测试：   
在虚拟机运行：sudo python -m SimpleHTTPServer 8001 # 不能使用映射端口   
在host打开浏览器访问 http://192.168.0.4:8001

### 3.3 private_network 私有网络
-

创建一个虚拟机与主机之间的私有网络，你可以为虚拟机手工指定一个 IP 地址，这个 IP 地址只有主机能访问到。

	# config.vm.network "public_network"  # 必须先关闭公网
	config.vm.network "private_network", ip: "192.168.33.10"

你可以修改成任何的为私有网络保留的地址段里的 IP 地址，不过不能使用跟电脑内网一样的地址段，比如我的路由器上设置的内网的 IP 地址段是 192.168.1.x ，这样为这个虚拟机配置私有网络地址的时候，就不能用 192.168.1.x 这个地址段上的 IP 地址。

测试：   
在虚拟机运行：sudo python -m SimpleHTTPServer 8001 # 不能使用映射端口.  
在host打开浏览器访问 http://192.168.33.10:8001

## 4 多主机
-

### 4.1 共享目录
-

[ref](https://ninghao.net/blog/2136)

	# 把下面的内容添加到vagrantfile里面，data是host上项目目录的相对目录，而vagrant_data则是guest的绝对路径，owner和group是目录所属的user owner和user group，下面的内容可以在逗号后面拆成两行。
	
	config.vm.synced_folder "./data", "/vagrant_data”, owner: "root", group: "root"

OR

	config.vm.synced_folder "./data", "/vagrant_data”, 
              owner: "root", group: "root"

用于共享的host上的目录必须事先创建好。

### 4.2 多主机
-

	config.vm.define “node1” do |node1| # 两个竖线之间是属性
	end

	config.vm.define “node2” do |node2| # 两个竖线之间是属性
	end

这种空配置下，两个机器的ip是一样的，上面的共享目录属性的对所有节点有效的。但是，不能再使用单个虚拟机情况下的网络设置。

### 4.3 多主机配置
-

	config.vm.define “node1” do |node1| # 两个竖线之间是属性
	    node1.vm.network “private_network”, ip: “192.168.113.11”  # 此处ip地址不能与host的ip处于同一个网段，也不能在ip最后一段使用1，这个数字被认为是router的ip地址。
	    node1.vm.hostname = “vagrant-node1”
	    node1.vm.synced_folder “./node1”, “/vagrant”
	end
	
	config.vm.define “node2” do |node2| # 两个竖线之间是属性
	    node2.vm.network “private_network”, ip: “192.168.113.12”
	    node2.vm.hostname = “vagrant-node2”
	    node2.vm.synced_folder “./node2”, “/vagrant”
	end
	
	也可以用如下方式通过loop一次定义多个guest：
	
	Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
	
	  (1..3).each do |i|
	    config.vm.define vm_name = “cdh#{i}”  do |config|
	        config.vm.provider “virtualbox” do |v|
	            v.customize [“modifyvm”, :id, “—name”, vm_name, “—memory”, “2048”,’—cpus’, 1]
	        end
	        config.vm.box = “centos6.5”
	        config.vm.hostname =vm_name
	        config.ssh.username = “vagrant”
	        config.vm.network :private_network, ip: “192.168.56.12#{i}”
		  	config.vm.provision :shell, :path => “bootstrap.sh”
	    end
	  end
	end

上面的文件中定义了三个虚拟机，三个虚拟机的名字和 hostname 分别为cdh1、cdh2、cdh3，网络使用的是 host-only 网络。

## 5 扩充磁盘空间
-

思路：一共两步，第一步把虚拟机的磁盘调大，第二步把文件系统（这里也就是分区）调大。调大磁盘的方式可以通过vagrant modifyhd指令实现，但是貌似只支持vdi格式的，必要的话找一下转化格式的方法。（另一个思路是通过virtualbox的图形界面去调整，可以尝试一下，还可以用--base打包）调整文件系统的话可以搜索一下linux调整分区。

### 5.1 外加一个磁盘
-

#### 5.1.1 plugin
-

    # vagrant plugin install vagrant-disksize
    config.disksize.size = ‘35GB’
    # vagrant plugin install vagrant-persistent-storage
    config.persistent_storage.enabled = true
    config.persistent_storage.location = “~/tmp/cdh.vdi”
    config.persistent_storage.size = 1024000 # 10G
    config.persistent_storage.mountname = ‘cloudera’
    config.persistent_storage.filesystem = ‘ext2’
    config.persistent_storage.mountpoint = ‘/opt/cloudera/‘
    config.persistent_storage.volgroupname = ‘myvolgroup’
    config.persistent_storage.diskdevice = ‘/dev/sdc’

这个方案缺点就是无法安装第二个extra盘，详细内容参考
https://github.com/kusnier/vagrant-persistent-storage/issues/22
参考内容里 dseevr 回答可做参考。

#### 5.1.2 原生命令
-

	Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
	
	  (0..0).each do |i|
	    config.vm.define vm_name = “node#{i}”  do |config|
	        config.vm.provider “virtualbox” do |v|
	            v.customize [“modifyvm”, :id, “—name”, vm_name, “—memory”, “3072”,’—cpus’, 2]
	            file_to_disk = File.realpath( “.” ).to_s + “/disk.vdi”
	            if ARGV[0] == “up” && ! File.exist?(file_to_disk)
	                puts “Creating 10GB disk #{file_to_disk}.”
	                v.customize [
	                     ‘createhd’,
	                     ‘—filename’, file_to_disk,
	                     ‘—format’, ‘VDI’,
	                     ‘—size’, 10000 * 1024 # 10 GB
	                     ]
	                v.customize [
	                     ‘storageattach’, :id,
	                     ‘—storagectl’, ‘SATA Controller’,
	                     ‘—port’, 1, ‘—device’, 0,
	                     ‘—type’, ‘hdd’, ‘—medium’,
	                     file_to_disk
	                     ]
	            end
	        end
	        config.vm.box = “cdh”
	        config.disksize.size = ‘35GB’
	        config.vm.hostname = vm_name
	        config.ssh.username = “vagrant”
	        config.vm.network :private_network, ip: “192.168.100.10#{i}”
	        config.vm.provision :shell, :path => “bootstrap.sh”
	    end
	  end
	end

### 5.2 扩大原盘size
-

[ref](https://gist.github.com/christopher-hopper/9755310)

* 关闭正在运行的机器：vagrant halt
* 查看vmdk信息 VBoxManage showvminfo node0  # 找到node0的磁盘目录 cd ~/VirtualBox\ VMs/node0 VBoxManage showvminfo node0 | grep ".vmdk"
* 转换vmdk格式 # 因为vmdk无法扩展size，并且可以留作备份 VBoxManage clonehd box-disk001.vmdk "clone-disk1.vdi" --format vdi
* 查看新生成文件信息 VBoxManage showhdinfo "clone-disk1.vdi" UUID:           ff55a4c9-b0c0-4863-8c26-daa47ee9f289 Parent UUID:    base State:          created Type:           normal (base) Location:       /Volumes/Macintosh HD/test/vm/node0/clone-disk1.vdi Storage format: VDI Format variant: dynamic default Capacity:       22528 MBytes Size on disk:   8893 MBytes Encryption:     disabled
* 改变vdi文件size VBoxManage modifyhd "clone-disk1.vdi" --resize 30720 # 30 * 1024M = 30G 这一步仅仅是扩展逻辑空间大小，物理空间不变。当有需要的时候vm才会扩展。
* 查看原vm信息 VBoxManage showvminfo node0 | grep "Storage"
* 根据信息提示，把新的vdi文件附加到原vm磁盘上 VBoxManage storageattach node0 --storagectl "SATA Controller" --port 0 --device 0 --type hdd --medium clone-disk1.vdi
* 重启虚机 vagrant up 
* 运行df查看磁盘情况 或者 启动virtualbox查看虚机的磁盘空间  以上过程效果等同plugin vagrant-disksize的配置：
        config.disksize.size = '30GB'


## 6 主机ssh登录失败

* 登录vagrant机器：vagrant ssh，使用默认密码vagrant。
* 创建ssh密钥：例如ssh-keygen -t rsa -b 4096 -C "vagrant"
* 重命名公钥文件(默认为id_rsa.pub)，覆盖旧的公钥文件：mv .ssh/id_rsa.pub .ssh/authorized_keys。
* 在需要的情况下重新加载ssh服务：sudo service ssh reload。
* 将私钥文件(默认id_rsa)复制到主机
* 在配置文件vagrantfile中添加config.ssh.private_key_path = "path/to/private_key"

或者更简单的使用sshpass，可以从任何地方登录虚机

## 7 为虚机centos安装desktop
	
	sudo yum groupinstall ‘X Window System’  -y
	sudo yum groupinstall “Desktop”
	sudo yum groupinstall -y “Chinese Support”

在/etc/inittab添加如下内容:
id:3:initdefault:

卸载命令如下:

	sudo yum groupremove “Chinese Support”
	sudo yum groupremove -y “Desktop”
	sudo yum groupremove -y “X Window System”
可以通过yum grouplist命令查看桌面相关的组件是否都在。


## 8 把virutalbox虚机转换为vmware虚机
-

[ref](https://gist.github.com/biophonc/2eded447d1f98ebdda34)

Export from VirtualBox

	1. Open Oracel VM VirtualBox Manager
	2. Make sure the VM is powered off
	3. Go to ‘File’ > ‘Export Appliance’, select your instance and hit ‘next’.
	4. Change destination folder for the *.ova if you like
	5. Set Format to ‘OVF 1.0’ and push ‘next’
	6. Change some Appliance Settings if you like. Hit the ‘export button’ and get a coffee

Import into VMware

	1. Go to ‘add’ > ‘import’ and select your previouly created .ova image
	2. Change the destination folder if you like and push ‘save’
    * VMware may inform you that the image does not passed the OVF specs - ignore it an retry. It should work anyway.

	# 以下步骤其实没有必要，因为vagrant vmware plugin是要收费的，大概79美刀。

Prepare box

	1. Go to location where your VMNAME.vmwarevm exists in cd into it.
	2. Make sure a ‘metadata.json’ file is in it. Otherweise create it. The minimum content should be: {“provider”:”vmware_fusion”} For more info read: docs.vagrantup.com/v2/boxes/format.html
	3. run tar cvzf NAME.box ./*to pack it. (This may take a while, so get a coffee)

Use it with Vagrant

	1. Backup the ‘Vagrantfile’ and the ‘.vagrant/‘’ folder
	2. Comment all providespecific settings in you Vagrantfile
	3. To add the box run: vagrant box add /path/to/VMNAME.vmwarevm/NAME.box —provider=vmware_fusion —name=NAME(Relax and enjoy your coffee)

	# The output should look something like this:
	
	b21:htdocs biophonc$ vagrant box add /Virtual-Machines/pp.vmwarevm/pp.box —provider=vmware_fusion —name=pp
	==> box: Adding box ‘pp’ (v0) for provider: vmware_fusion
	    box: Downloading: file:///Virtual-Machines/pp.vmwarevm/pp.box
	==> box: Successfully added box ‘pp’ (v0) for ‘vmware_fusion’!


