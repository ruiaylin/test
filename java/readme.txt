project-starter.tar.gz isnail的原始模板

youni_java_project_template.bz2 修改后可以使用的模板

编译脚本build.sh内容如下，-P参数后面的funtest指定的是profile的内容
mvn -U -DskipTests clean package -Pfuntest%

