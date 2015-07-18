                      MyICQ 0.8 alpha1测试版
             ====================================================

---------------
1. MyICQ是什么？
---------------
    MyICQ是一套公开源代码的即时通讯软件，包括服务器端和客户端，可以用于互联网或局域网中。可以运行在Windows或Linux(KDE/Qt)操作系统上，这是Windows版。目前客户端程序的界面完全模仿腾讯的QQ(如果Tencent告我的话，我会马上改的:-)。
    总之，如果你崇尚自由，对QQ的越来越多的广告骚扰感到深恶痛绝的话，MyICQ绝对是你很好的选择。


--------
2. 版本
--------
这是MyICQ 0.8版本的alpha1 测试版，基本上还没有经过什么严格测试。


--------
3. 版权
--------
完全遵循GPL协议2.0或以后协议版本。


---------------
4. 基本功能特点
---------------
1) 收发(离线)消息(如果客户端之间能直接通讯，则通过UDP协议发送，否则通过服务器中转)
2) 添加/删除好友(可以设置身份验证)
3) 服务器端存储好友列表
4) 在客户端存储好友资料和聊天记录
5) 客户端与服务器端用DES的密钥加密方式通讯
6) 支持代理服务器(SOCKS5/HTTP)
7) 向在线的一组好友发送消息
8) 系统管理员(MyICQ号 < 1000)可以发送系统广播消息
9) 皮肤系统
10)完全基于插件，使扩展更容易。目前提供的插件有:
   a. 收发文件
   b. 二人世界(可以实时语音聊天)
   c. 闹钟提醒


------------------
5. 分发包里有什么？
------------------
分发包里有三个目录:
myicq/		客户端程序
myicqd/		服务器端程序
myicqhttp/	HTTP的转换程序。MyICQ支持HTTP代理，但需要在MyICQ的服务器端运行myicqhttp程序


--------
6. 安装
--------
客户端程序不写Windows注册表，无需安装。双击MyICQ.exe运行。

服务器端由于内部采用MySQL数据库，所以安装稍微麻烦一些:
1) 到http://www.mysql.com下载MySQL
2) 打开一个DOS命令窗口，在MySQL中建立一个数据库，并添加一个用户:
   C:\mysql\bin> mysql -uroot -p
   ******** (输入root密码,如果还没有设置，直接输入回车即可)
   mysql> GRANT ALL ON myicq.* TO myicq@localhost IDENTIFIED BY 'myicq';
   mysql> CREATE DATABASE myicq;
   mysql> quit
3) 创建表格:
   C:\mysql\bin> mysql -umyicq -Dmyicq -p < [myicqd目录]\myicq.sql
   password: myicq
   注意: [myicqd目录]代表myicqd所在的目录
4) 运行myicqd:
   双击myicqd.exe即可运行。如果没有错误，应该显示"MyICQ server is now started".
   接下来就可以从客户端注册新用户了。
5) 你可能想要添加一个系统用户，以发送系统广播消息:
   C:\mysql\bin> mysql -umyicq -Dmyicq -p
   password: myicq
   mysql> INSERT INTO basic_tbl (uin, passwd) VALUES(100, password('yourpassword'));
   mysql> INSERT INTO ext_tbl (uin) VALUES(100);
   注意: yourpassword代表系统用户的密码
   然后选择客户端程序的注册向导，取回100这个号码。完成后，你会发现在主菜单中多了"广播消息"一项。此后，你可以选择个人设定来设定你的个人信息（比如，不允许任何人加我为好友)

   
-------------
7. 编译源代码
-------------
如果你是一个程序员，那么一定要编译MyICQ的源代码:-)
1) 到http://www.mysql.com下载MySQL
2) 由于MyICQ在存储好友资料和聊天记录时，使用Berkeley DB库，所以先到
   http://www.sleepycat.com下载
3) 编译Berkeley DB
4) 在VC的Tools->Options->Directories中设置好MySQL和BerkeleyDB的include和library路径
5) 在VC中打开myicq-win32/src/win32/myicq.dsw项目文件
6) 编译


---------
8. TODO
---------
1) POP3邮件自动监测(插件)
2) 文件共享(插件)
3) 一些小游戏(插件，如俄罗斯方块)
4) 发送好友/URL消息
5) 更完善的皮肤系统
6) 服务器端改为Windows服务程序
7) 服务器端的性能和稳定性


-----------
9. 共同开发
-----------
    MyICQ是我利用上学和上班的空闲时间编写的，发展到现在已经是一个比较大的工程了，以我一个人的力量继续编写下去将非常困难。希望广大的程序员高手能够参与到这个项目中来，为自由软件贡献一份力量。
我在sourceforge.net上申请了CVS服务空间，项目名是myicq-free。有意加入者请与我联系。


-----------
10. 联系方式
-----------
如果你在使用MyICQ的过程中，遇到了什么BUG，或是有什么新的想法和建议，或是喜欢这个程序，我将非常高兴收到你的来信。

Email: z-yong163@163.com
张勇

Linux用户请不要着急，等在Windows下测试稳定后，马上就会出Linux版的！



天才的人是流星，注定要燃烧自己以照亮他们的世纪
                                               ----- 拿破仑波拿巴
一辆法拉利小车只是物质享受，做人最重要的是真诚
                                               ----- John Carmack
什么叫专业，这就...
                                               ----- 张勇:-)