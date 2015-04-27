#!/bin/sh
#python backup.py publicdb.bce-preinternal.baidu.com 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs /home/gm/scs/data_backup
case C"$1" in
	C)
		echo "Usage: $0 {backup|restore}"
		;;
	Cbackup)
		if [ $# != 7 ]; then
			echo "Please Input: dbhost dbport dbuser dbpassword dbname localdir"
		else
                        python backup.py $2 $3 $4 $5 $6 $7
			echo "Done!"
		fi
		;;
	Crestore)
		if [ $# != 12 ]; then
			echo "Please Input: dbhost dbport dbuser dbpassword dbname localdir src_cluster_id src_ip dst_cluster_id dst_ip dst_dir"
		else
                        python restore.py ${2} ${3} ${4} ${5} ${6} ${7} ${8} ${9} ${10} ${11} ${12}
			echo "Done!"
		fi
		;;
	C*)
		echo "Usage: $0 {backup|restore}"
		;;
esac
