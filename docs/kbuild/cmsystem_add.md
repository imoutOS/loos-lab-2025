# 编译 kernel 子系统如何添加一个目录/文件

### 添加一个新的目录到编译子系统

#### topMakefile 里面怎么添加

1. 找到并打开topMakfile 路径在 **path2project/Makefile**

2. 找到第一个 if 的 else 分支 ，搜索

   ```shell
   else #### end for first call
   ```

3. 可以看到下面内容

   ```makefile
   else #### end for first call
   
   ### when call this Makefile for the second time
   ### it is time to start our compile
   obj-y += core/
   obj-y += driver/
   obj-y += fs/
   obj-y += lolibc/
   obj-y += mm/
   obj-y += arch/
   endif #### end for other call
   ```

4. 需要添加的内容在 else -> endif 之间

5. 添加 **obj-y += 目录名/** ，此处以目录名为 test 举例，添加完成后如下

   ```makefile
   else #### end for first call
   
   ### when call this Makefile for the second time
   ### it is time to start our compile
   obj-y += core/
   obj-y += driver/
   obj-y += fs/
   obj-y += lolibc/
   obj-y += mm/
   obj-y += arch/
   obj-y += test/
   endif #### end for other call
   ```

   至此，已经将 test 目录添加进编译子系统的范畴

   

#### 通用Makefile 里面怎么添加

1. 找到并打开一个在编译子系统范畴里面的 Makefile ，此处以上面的 test 目录为例子，把 test 目录添加进入范畴后，需要在 test 目录下创建名称为 Makefile 的文件，此时 Makefile 为空

2. 直接添加 添加 **obj-y += 目录名/** ，此处以目录名为 abc 举例，添加完成后如下

   ```makefile
   obj-y += abc/
   ```





### 添加一个新的文件到编译子系统

#### topMakefile 里面怎么添加

1. 找到并打开topMakfile 路径在 **path2project/Makefile**

2. 找到第一个 if 的 else 分支 ，搜索

   ```shell
   else #### end for first call
   ```

3. 可以看到下面内容

   ```makefile
   else #### end for first call
   
   ### when call this Makefile for the second time
   ### it is time to start our compile
   obj-y += core/
   obj-y += driver/
   obj-y += fs/
   obj-y += lolibc/
   obj-y += mm/
   obj-y += arch/
   endif #### end for other call
   ```

4. 需要添加的内容在 else -> endif 之间

5. 添加 **obj-y += 文件名.o** ，此处**以想要编译的文件为 aaa.c** 举例，添加完成后如下

   ```makefile
   else #### end for first call
   
   ### when call this Makefile for the second time
   ### it is time to start our compile
   obj-y += core/
   obj-y += driver/
   obj-y += fs/
   obj-y += lolibc/
   obj-y += mm/
   obj-y += arch/
   obj-y += aaa.o
   endif #### end for other call
   ```

   至此，已经将 aaa.c 文件添加进编译子系统的范畴

   

#### 通用Makefile 里面怎么添加

1. 找到并打开一个在编译子系统范畴里面的 Makefile ，此处以上面的 test 目录为例子，把 test 目录添加进入范畴后，需要在 test 目录下创建名称为 Makefile 的文件，此时 Makefile 为如下

   ```makefile
   obj-y += abc/
   ```

2. 直接添加 添加 **obj-y += 文件名.o** ，此处以**文件名为 jkl.c** 举例，添加完成后如下

   ```makefile
   obj-y += abc/
   obj-y += jkl.o
   ```



### 



