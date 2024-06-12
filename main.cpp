#include "database.h"
#include <iostream>
#include <string>
#include <vector>

void developerMenu(Database& db, int user_id);
void adminMenu(Database& db);
void sysadminMenu(Database& db);

void login(Database& db, int role_id) {
    std::string username, password;
    int user_id;
    std::cout << "账号：";
    std::cin >> username;
    std::cout << "密码：";
    std::cin >> password;

    if (db.verifyUser(username, password, role_id, user_id)) {
        if (role_id == 1) {
            developerMenu(db, user_id);
        }
        else if (role_id == 2) {
            adminMenu(db);
        }
        else if (role_id == 3) {
            sysadminMenu(db);
        }
    }
    else {
        std::cout << "验证失败，请重试。" << std::endl;
    }
}

void developerMenu(Database& db, int user_id) {
    int choice;
    do {
        std::cout << "1. 申请预约\n";
        std::cout << "2. 查看自己预约\n";
        std::cout << "3. 取消预约\n";
        std::cout << "4. 退出\n";
        std::cout << "选择: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            db.listMeetingRooms();
            std::string date, time_slot;
            std::cout << "输入预约日期 (YYYY-MM-DD): ";
            std::cin >> date;
            std::cout << "输入时间段 (morning/afternoon): ";
            std::cin >> time_slot;

            std::vector<std::string> status = db.getRoomStatus(date, time_slot);
            for (const auto& room_status : status) {
                std::cout << room_status << std::endl;
            }

            int room_id;
            std::cout << "输入会议室编号: ";
            std::cin >> room_id;
            if (!db.isRoomAvailable(room_id, date, time_slot)) {
                std::cout << "预约失败，会议室已被占用。" << std::endl;
            }
            else {
                bool approval_required = db.isSpecialRoom(room_id);
                std::string status = approval_required ? "pending" : "approved";
                std::string sql = "INSERT INTO reservations (user_id, room_id, date, time_slot, status, is_deleted) VALUES (" +
                    std::to_string(user_id) + ", " + std::to_string(room_id) + ", '" + date + "', '" + time_slot + "', '" + status + "', 0)";
                db.execute(sql);
                if (approval_required) {
                    std::cout << "预约成功，等待审批。" << std::endl;
                }
                else {
                    std::cout << "预约成功！" << std::endl;
                }
            }
            break;
        }
        case 2: {
            db.listUserReservations(user_id);
            break;
        }
        case 3: {
            int reservation_id;
            std::cout << "输入要取消的预约编号: ";
            std::cin >> reservation_id;
            db.cancelReservation(reservation_id, user_id);
            break;
        }
        case 4: {
            std::cout << "退出登录" << std::endl;
            break;
        }
        default: {
            std::cout << "无效选择，请重试。" << std::endl;
        }
        }
    } while (choice != 4);
}

void adminMenu(Database& db) {
    int choice;
    do {
        std::cout << "1. 查看所有会议室预约信息\n";
        std::cout << "2. 审批预约\n";
        std::cout << "3. 退出\n";
        std::cout << "选择: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            db.listAllReservations();
            break;
        }
        case 2: {
            int reservation_id;
            bool approve;
            db.listAllReservations();
            std::cout << "输入预约编号: ";
            std::cin >> reservation_id;
            std::cout << "输入审批结果 (1: 通过, 0: 不通过): ";
            std::cin >> approve;
            std::string status = ((approve==1) ? "approved" : "rejected");
            std::string sql = "UPDATE reservations SET status = '" + status + "' WHERE id = " + std::to_string(reservation_id);
            db.execute(sql);
            std::cout << "审批完成。" << std::endl;
            break;
        }
        case 3: {
            std::cout << "退出登录" << std::endl;
            break;
        }
        default: {
            std::cout << "无效选择，请重试。" << std::endl;
        }
        }
    } while (choice != 3);
}

void sysadminMenu(Database& db) {
    int choice;
    do {
        std::cout << "1. 添加账户\n";
        std::cout << "2. 查看账户\n";
        std::cout << "3. 查看会议室信息\n";
        std::cout << "4. 清空预约\n";
        std::cout << "5. 退出\n";
        std::cout << "选择: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::string username, password;
            int role_id;
            std::cout << "输入用户名: ";
            std::cin >> username;
            std::cout << "输入密码: ";
            std::cin >> password;
            std::cout << "选择角色 (1: 研发人员, 2: 行政人员): ";
            std::cin >> role_id;
            db.addUser(username, password, role_id);
            break;
        }
        case 2: {
            int role_id;
            std::cout << "选择角色 (1: 研发人员, 2: 行政人员): ";
            std::cin >> role_id;
            db.listUsersByRole(role_id);
            break;
        }
        case 3: {
            db.listMeetingRooms();
            break;
        }
        case 4: {
            db.clearOldReservations();
            break;
        }
        case 5: {
            std::cout << "退出登录" << std::endl;
            break;
        }
        default: {
            std::cout << "无效选择，请重试。" << std::endl;
        }
        }
    } while (choice != 5);
}

int main() {
    Database db;
    int choice;
    do {
        std::cout << "1. 研发人员登录\n";
        std::cout << "2. 行政人员登录\n";
        std::cout << "3. 系统管理员登录\n";
        std::cout << "4. 退出\n";
        std::cout << "选择身份: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            login(db, 1);
            break;
        }
        case 2: {
            login(db, 2);
            break;
        }
        case 3: {
            login(db, 3);
            break;
        }
        case 4: {
            std::cout << "退出系统" << std::endl;
            break;
        }
        default: {
            std::cout << "无效选择，请重试。" << std::endl;
        }
        }
    } while (choice != 4);

    return 0;
}
