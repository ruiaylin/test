# git

## blame
> git blame filename # 可以查看某个文件的详细修改历史

## branch
* git branch # 列出本地的分支
* git branch -vv  # 列出分支并查看每个分支的详情
* git branch -av # 查看远程所有分支
* git branch -r # 查看远程分支

* git checkout branch-name # 切换到某个分支
* git checkout -b branch-name # 创建某个分支
* git checkout -b hotfix origin/hotfix # 切换到远程分支 origin/hotfix，本地名称叫做hotfix
* git status # 查看分支情况
* git branch -d branch-name # 尝试删除本地某个分支(删除分支之前一定要先切换到别的分支上面)
* git branch -D <branchname>：强制删除某个分支
* git push origin branch-name  # 向远程某个分支提交代码
* git branch -m old new # 重命名本地分支名称

* git push origin :branch-name   冒号前面的空格不能少，原理是把一个空分支push到server上，相当于删除该分支。

* git branch -m <oldbranchname> <newbranchname>：尝试修改
* git branch -M <oldbranchname> <newbranchname>：强制修改

## commit
* git commit --amend   # 可以修改最后一次提交的信息.但是如果你已经push过了，那么其历史最后一次，永远也不能修改了。 这种方式可以比较方便的保持原有的Change-Id，推荐使用。
* git commit -am “some sthing” 这个命令会让当前在暂存区里面的文件修改保存并且 commit ，但是 untracted files (新创建还没有被git add的文件)不会被加入到仓库区。[m与am的区别](http://www.cnblogs.com/xiaohuochai/p/6664451.html)

## config   
* git config -l # 查看本地配置

>  编辑代码库根目录.git/config，增加GitLab 远程库

[remote "gitlab"]   
    url = git@1.2.1.1:l/a.git   
    fetch = +refs/heads/*:refs/remotes/gitlab/*   
    pushurl = git@1.2.1.1:l/a.git   
    pushurl = git@1.2.1.1:l/b.git

上面设置两个pushurl，用于同时推送到两个gitlab上，也可以通过如下两个命令达到同样的效果：   
git remote set-url --add --push origin git@1.2.1.1:l/a.git   
git remote set-url --add --push origin git@1.2.1.1:l/b.git 

> global

* git config --global color.ui true # 使语法着色，让命令更加突出
* git config --list # 查看config信息
* git config --global user.name "shenguotao"
* git config --global user.email "sgt_ah@163.com"
* git config --global alias.logg "log --graph --decorate --abbrev-commit --all" # 美化log

> alias
  
* git config --global alias.st status
* git config --global alias.co checkout
* git config --global alias.ci commit
* git config --global alias.br branch
* git config --global alias.pl pull
* git config --global alias.ps push


## merge

> example1: 
  
*  git checkout develop   
*  git branch -av
*  git merge --no-ff feature/actlog # 强制merge
*  git branch -av
*  git push origin

> example2: 

* git checkout b
* git fetch origin # 拉取origin远程所有信息，merge之前一定先确保本地代码是最新的
* git pull 
* git merge a # 把a merge进本地b
* git status # merge 完毕之后，通过这个命令查看不同的文件，然后手工修改
* git add xxx # 手工修改之后，add每个diff的文件
* git commit -m “”
* git push origin head　#  把本地最新代码提交上去

## mv
* git mv -f oldfolder newfolder # 重命名文件夹 or 文件

## log & show

* git log -p # 查看提交的详细内容
* git show log-id # 用于查看某次提交前后代码的diff，log-id可以是提交id的前五位

## reset

* git reset --hard commit-id # commit-id可以通过git log查看，这个命令的意思是会退到相关的commit，把最新的提交撤销掉，彻底回退到某个版本，本地的源码也会变为上一个版本的内容
* git reset —soft # 回退到某个版本，只回退了commit的信息，不会恢复到index file一级。如果还要提交，直接commit即可
* git reset --mixed # 此为默认方式，不带任何参数的git reset，即时这种方式，它回退到某个版本，只保留源码，回退commit和index信息

## push & pull

* git push -f master_zhao:master
* git push <远程主机名> <本地分支名>:<远程分支名>

* git pull  <远程主机名> <远程分支名>:<本地分支名> # 用于拉取位于服务器上的最新分支到本地并合并，它等效于 git fetch + git merge。命令git fetch 会拉取服务器上的分支并保存在版本库的某个文件夹下，命令git merge则会将拉取下来的版本库与本地版本库进行合并。

## rebase

* git rebase -i HEAD~4 # 将最近4个commit合并为1个，HEAD代表当前版本。将进入VIM界面，你可以修改提交信息。

## remote

* git remove -v  这个命令可以显示对应项目的远程克隆地址
* git remote add origin git@github.com:Programer-yang/TestGit.git # 这里的远程仓库名字，我们默认为origin，当然也是可以修改的，这个远程仓库的简称是为了简单替代长串的URL，在以后的推送和拉取中就可以使用这个简称来简化命令了。本地repo可以关联多个远程repo。

## rm file

*   git rm a.txt
*   git rm -rf a.txt
*   git commit * -m "update"
*   git push origin master

## stash

* git stash  # 保存当前分支的工作状态
* git stash list # 查看当前分支的工作状态列表
* git stash pop # 恢复分支最新的工作状态
* git stash appy stash@{0} # 恢复分支指定的工作状态,该命令可结合 git stash list 使用
* git stash drop # 删除分支状态

## tag

* git tag v0.4.05     # 创建本地tag
* git push origin v0.4.05 # 创建远程tag
* git tag -d v0.4.05 # 删除本地tag
* git push origin :refs/tags/v0.4.05 # 删除远程tag

* git tag <tagName> [commitID]. # 为指定的commit创建标签
* //示例：git tag v0.9 6224937
* git tag -a <tagName> -m 'note content' # 创建带注释说明的标签

## upstream

* git remote -v # 可以查看上游仓库
* git remote add upstream https://github.com/antirez/redis.git # 配置remote
* git fetch upstream # 从上游仓库获取到分支，及相关的提交信息，它们将被保存在本地的 upstream/master 分支
* git checkout master # 切换到本地的一个分支
* git merge upstream/master # 合并到本地的当前分支
* git ci -am “xxx”
* git push origin head # 提交到fork仓库

## questions

> add Existing folder or Git repository

* cd existing_folder
* git init
* git remote add origin git@101.201.155.81:zx/ps.git
* git add .
* git commit
* git push -u origin master

> 在GitLab添加SSH key   

* 参考https://docs.gitlab.com/ce/gitlab-basics/create-your-ssh-keys.html

> push代码的时候遇到protect提示 “You are not allowed to force push code to a protected branch on this project”

* “settings -> Protected Branches” 在这里面可以对相关branch是否处于protect模式进行设置。

> 免密

首先配置~/.ssh/config，然后配置代码repo下的.git/config:

[remote "origin"]
    url = https://github.com/alexstocks/   
    fetch = +refs/heads/*:refs/remotes/origin/*

修改为：

[remote "origin"]   
    url = git@github:AlexStocks/test
    fetch = +refs/heads/*:refs/remotes/origin/*

<font color=red>**把https方式push改为ssh方式push，此时.ssh下面的配置就起作用了。**</font>

> git整体存储框架

![](../pic/git/repo_storage.png)

> git 存储架构与命令操作过程   

![](../pic/git/repo_storage_command.png)