此目录存放本周课后作业，可以在此文件添加作业设计思路和流程图等

## 数据库文件：meeting_manegament.sql

## 流程图：process.html

## 单元测试文档：foo.xml



## 内存泄漏：确保所有通过 new 分配的内存都能通过 delete 释放。
## 智能指针：优先使用智能指针管理资源，以便自动管理内存。
## 异常处理：确保在异常情况下也能正确释放资源。

删除数据时在逻辑上删除，表中依旧存在

## 单元测试控制台输出：
[==========] Running 7 tests from 2 test cases.
[----------] Global test environment set-up.
[----------] 1 test from TestCaseName
[ RUN      ] TestCaseName.TestName
[       OK ] TestCaseName.TestName (0 ms)
[----------] 1 test from TestCaseName (0 ms total)

[----------] 6 tests from DatabaseTest
[ RUN      ] DatabaseTest.VerifyUserTest
[       OK ] DatabaseTest.VerifyUserTest (79 ms)
[ RUN      ] DatabaseTest.AddAndVerifyReservation
1
1
1
1
1
您不能取消其他人的预约。
[       OK ] DatabaseTest.AddAndVerifyReservation (19 ms)
[ RUN      ] DatabaseTest.CancelReservationTest
您不能取消其他人的预约。
[       OK ] DatabaseTest.CancelReservationTest (19 ms)
[ RUN      ] DatabaseTest.IsRoomAvailableTest
您不能取消其他人的预约。
[       OK ] DatabaseTest.IsRoomAvailableTest (21 ms)
[ RUN      ] DatabaseTest.AddUserTest
执行失败: Duplicate entry 'newuser213' for key 'username'
用户已添加。
[       OK ] DatabaseTest.AddUserTest (21 ms)
[ RUN      ] DatabaseTest.ListUsersByRoleTest
用户编号: 1, 用户名: 1, 角色编号: 1
用户编号: 104, 用户名: newuser, 角色编号: 1
用户编号: 107, 用户名: newuser1, 角色编号: 1
用户编号: 243, 用户名: newuser213, 角色编号: 1
用户编号: 305, 用户名: testuser, 角色编号: 1
用户编号: 2, 用户名: 2, 角色编号: 2
用户编号: 108, 用户名: newuser2, 角色编号: 2
用户编号: 306, 用户名: adminuser, 角色编号: 2
[       OK ] DatabaseTest.ListUsersByRoleTest (27 ms)
[----------] 6 tests from DatabaseTest (188 ms total)

[----------] Global test environment tear-down
[==========] 7 tests from 2 test cases ran. (192 ms total)
[  PASSED  ] 7 tests.

D:\code\cpp\jinShanTraining\Test\Project2\x64\Release\Sample-Test1.exe (进程 21728)已退出，代码为 0。
要在调试停止时自动关闭控制台，请启用“工具”->“选项”->“调试”->“调试停止时自动关闭控制台”。
按任意键关闭此窗口. . .