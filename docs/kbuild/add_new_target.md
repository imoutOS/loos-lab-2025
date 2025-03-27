# 如何在Makefile 中添加一个新的目标

## 前言

如[用户编译指南](./User_use.md)中所指出，我们可以通过 make help 检查当前可使用的所有的目标集合，此文档会讲述我们如何在 Kbuild System 中添加一个新的目标，并给出一些添加的规范

## 正文

### 添加新的目标

1. 在 **path2project/Makefile** 文件里面找到**预留给后续添加目标的位置**，一般是下面的格式

   ```shell
   ###################################### target-x xxxxxxxx xxxx ###########################################
   ```

2. 修改 target 名称，延续上面的target ，如

   ```shell
   ###################################### target-4 Makefile help ###########################################
   空一行 （尽量保持这个规范，阅读起来方便）
   ....
   ....
   ....
   空一行 （尽量保持这个规范，阅读起来方便）
   下面就是要添加的 target （注：尽量保持长度一致，阅读起来方便）
   ############################### target-5 你想添加的目标的大致描述 ###########################################
   ```

   Note  ： 尽量使用英文来描述添加的目标

3. **重要：往下预留下一个目标的位置**，方便下一个添加目标的人找到该位置，做如下操作

   ```shell
   ############################### target-5 你想添加的目标的大致描述 ############################################
   空一行 （尽量保持这个规范，阅读起来方便）
   ....
   ....
   ....
   空一行 （尽量保持这个规范，阅读起来方便）
   ###################################### target-x xxxxxxxx xxxx ###########################################
   ```

4. 添加目标规则 

   1. [见如下](##规则添加)

5. **参与 help 目标的耦合**，在 **help 目标**中添加对应的需要 port 给用户的目标以及其功能( 如果不需要则不添加 )，下面是举例

   ```makefile
   help:
           @echo  'Cleaning targets:'
           @echo  '  clean           - Remove most generated files but keep the config and'
           @echo  '                    enough build support to'
           @echo  '  distclean       - mrproper + remove editor backup and patch files'
           @echo  ''
           @echo  'Other generic targets:'
           @echo  '  compile         - Build the bare kernel'
           @echo  ''
           @echo  '  menuconfig      - Configure Kernel config'
           @echo  ''
   ```

   原 help 目标如上，若添加了一个 test 目标应该改成如下

   ```makefile
   help:
           @echo  'Cleaning targets:'
           @echo  '  clean           - Remove most generated files but keep the config and'
           @echo  '                    enough build support to'
           @echo  '  distclean       - mrproper + remove editor backup and patch files'
           @echo  ''
           @echo  'Other generic targets:'
           @echo  '  compile         - Build the bare kernel'
           @echo  ''
           @echo  '  menuconfig      - Configure Kernel config'
           @echo  ''
           @echo  '  test            - just a test target'
           @echo  ''
   ```

6. 添加文档说明，添加的变量添加到 [topMakefile 全局变量手册](./topMakefile_Var.md)、添加的新目标以及功能添加到 [topMakefile 规则手册](./topMakefile_Rule.md) 【此操作，可增加 Kbuild System 的可维护性，减少重复造轮子，如果我们需要添加一个新的目标，只需要在之前的某个目标前提下做某些简单的事情，可以查手册，然后将对应的老目标添加为依赖即可】



### 规则添加

1. 考虑该规则是否需要利用已经有的规则，查阅 [topMakefile 规则手册](./topMakefile_Rule.md)
2. 考虑该为则是否可以利用已经有的变量，查阅  [topMakefile 全局变量手册](./topMakefile_Var.md)
3. 考虑该规则是否为 PHONY 规则，若是，则需要添加 PHONY += xxx
4. 考虑该规则的内容是否足够简单
   1. 简单  -> 直接在 顶层 Makefile 完成
   2. 复杂  ->  分到 子Makefile 中进行



### 规则添加举例

- 简单规则

  - 内容 ： 添加 help 规则，将所有可用的规则输出到终端上去

  - 操作 ： 添加如下规则

    ```makefile
    PHONY+= help  (因为 help 是一个 PHONY 规则，所以我们有这么一行操作)
    help:
            @echo  'Cleaning targets:'
            @echo  '  clean           - Remove most generated files but keep the config and'
            @echo  '                    enough build support to'
            @echo  '  distclean       - mrproper + remove editor backup and patch files'
            @echo  ''
            @echo  'Other generic targets:'
            @echo  '  compile         - Build the bare kernel'
            @echo  ''
            @echo  '  menuconfig      - Configure Kernel config'
            @echo  ''
    ```

- 复杂规则

  - 内容  ： 添加 compile 规则

  - 操作  ： 添加如下规则

    ```makefile
    TARGET          = ${src_tree}/kernel (添加了新的变量，port 给子 Makefile 使用，生成的 kernel 的绝对路径)
    export TARGET
    
    ${TARGET}: kconfig_sync compile ${src_tree}/built-in.a (依赖了 kconfig文件生成规则、 compile(内部规则)、一个具体文件)
            $Q${LD} ${LDFLAGS} ${src_tree}/built-in.a -o $@
            @${ECHO} $(call QUIET_LINK,$@)
    
    PHONY+= compile （compile 是一个 PHONY 依赖）
    compile:
            $Q${MAKE} -C ${src_tree} -f ${src_tree}/scripts/Makefile.build (子Makefile 里面再去执行 具体的 compile 命令)
    ```



### Note

#### 因为在 topMakefile 中有一些与其他目标的耦合，所以建议的是更多的去执行 子Makefile （干净的 Makefile环境 + topMakefile 传递的变量）来做操作

添加完目标之后，我们的目标本应该是相对独立的情况，不影响其他目标，但是有一些情况是存在耦合的

必须的耦合： 

1.  直接参与到 help 目标的耦合 , help 目标用于输出用户可使用的目标以及目标的注释，我们需要在 help 目标中添加
2.  **目标规则使用了已经使用过的目标名称**、**添加的变量名称和之前的目标名称或者之前的变量冲突**等（我们需要避免这个情况）

特性耦合 :

1.  添加的新目标确实需要依赖之前的目标，比如需要编译出内核，则需要依赖 ${TARGET} ，需要产生 .config 文件需要依赖  kconfig_sync 等情况



