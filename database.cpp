#include "database.h"
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>
#include <iostream>
#include <vector>

Database::Database() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        connection.reset(driver->connect("tcp://localhost:3306", "root", "root"));
        connection->setSchema("meeting_management");

        // 设置字符集为 UTF-8
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        stmt->execute("SET NAMES 'utf8mb4'");
    }
    catch (sql::SQLException & e) {
        std::cout << "数据库连接失败: " << e.what() << std::endl;
    }
}

Database::~Database() {
    // 连接会自动关闭
}

bool Database::verifyUser(const std::string& username, const std::string& password, int role_id, int& user_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT id FROM users WHERE username = '" + username + "' AND password = '" + password + "' AND role_id = " + std::to_string(role_id);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        if (res->next()) {
            user_id = res->getInt("id");
            return true;
        }
        return false;
    }
    catch (sql::SQLException & e) {
        std::cout << "验证用户失败: " << e.what() << std::endl;
        return false;
    }
}

void Database::query(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
        while (res->next()) {
            std::cout << "会议室编号: " << res->getInt("id")
                << ", 名称: " << res->getString("name")
                << ", 容纳人数: " << res->getInt("capacity")
                << ", 需审批: " << (res->getBoolean("approval_required") ? "是" : "否")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "查询失败: " << e.what() << std::endl;
    }
}

void Database::execute(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        stmt->execute(sql);
    }
    catch (sql::SQLException & e) {
        std::cout << "执行失败: " << e.what() << std::endl;
    }
}

bool Database::isRoomAvailable(int room_id, const std::string& date, const std::string& time_slot) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT COUNT(*) AS count FROM reservations WHERE room_id = " +
            std::to_string(room_id) + " AND date = '" + date + "' AND time_slot = '" + time_slot + "' AND is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        res->next();
        return res->getInt("count") == 0;
    }
    catch (sql::SQLException & e) {
        std::cout << "检查房间可用性失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::isSpecialRoom(int room_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT approval_required FROM meeting_rooms WHERE id = " + std::to_string(room_id);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        if (res->next()) {
            return res->getBoolean("approval_required");
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "检查房间类型失败: " << e.what() << std::endl;
    }
    return false;
}

bool Database::isUserReservation(int reservation_id, int user_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT COUNT(*) AS count FROM reservations WHERE id = " + std::to_string(reservation_id) + " AND user_id = " + std::to_string(user_id) + " AND is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        res->next();
        return res->getInt("count") == 1;
    }
    catch (sql::SQLException & e) {
        std::cout << "检查预约所属失败: " << e.what() << std::endl;
        return false;
    }
}

void Database::cancelReservation(int reservation_id, int user_id) {
    try {
        if (isUserReservation(reservation_id, user_id)) {
            std::string query = "UPDATE reservations SET is_deleted = 1 WHERE id = " + std::to_string(reservation_id);
            execute(query);
            std::cout << "预约已取消。" << std::endl;
        }
        else {
            std::cout << "您不能取消其他人的预约。" << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "取消失败: " << e.what() << std::endl;
    }
}

std::vector<std::string> Database::getRoomStatus(const std::string& date, const std::string& time_slot) {
    std::vector<std::string> status;
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT r.id, r.name, r.capacity, r.approval_required, "
            "(SELECT COUNT(*) FROM reservations res WHERE res.room_id = r.id AND res.date = '" + date + "' AND res.time_slot = '" + time_slot + "' AND res.is_deleted = 0) AS booked "
            "FROM meeting_rooms r";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::string room_status = "会议室编号: " + std::to_string(res->getInt("id")) +
                ", 名称: " + res->getString("name") +
                ", 容纳人数: " + std::to_string(res->getInt("capacity")) +
                ", 需审批: " + (res->getBoolean("approval_required") ? "是" : "否") +
                ", 预定情况: " + (res->getInt("booked") > 0 ? "已预定" : "空闲");
            status.push_back(room_status);
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "获取会议室状态失败: " << e.what() << std::endl;
    }
    return status;
}

void Database::listUserReservations(int user_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM reservations WHERE user_id = " + std::to_string(user_id) + " AND is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "预约编号: " << res->getInt("id")
                << ", 会议室编号: " << res->getInt("room_id")
                << ", 日期: " << res->getString("date")
                << ", 时段: " << res->getString("time_slot")
                << ", 状态: " << res->getString("status")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "查询失败: " << e.what() << std::endl;
    }
}

void Database::listAllReservations() {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM reservations WHERE is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "预约编号: " << res->getInt("id")
                << ", 用户编号: " << res->getInt("user_id")
                << ", 会议室编号: " << res->getInt("room_id")
                << ", 日期: " << res->getString("date")
                << ", 时段: " << res->getString("time_slot")
                << ", 状态: " << res->getString("status")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "查询失败: " << e.what() << std::endl;
    }
}

void Database::addUser(const std::string& username, const std::string& password, int role_id) {
    try {
        std::string query = "INSERT INTO users (username, password, role_id) VALUES ('" + username + "', '" + password + "', " + std::to_string(role_id) + ")";
        execute(query);
        std::cout << "用户已添加。" << std::endl;
    }
    catch (sql::SQLException & e) {
        std::cout << "添加用户失败: " << e.what() << std::endl;
    }
}

void Database::listUsersByRole(int role_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM users WHERE role_id = " + std::to_string(role_id);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "用户编号: " << res->getInt("id")
                << ", 用户名: " << res->getString("username")
                << ", 角色编号: " << res->getInt("role_id")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "查询失败: " << e.what() << std::endl;
    }
}

void Database::listMeetingRooms() {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM meeting_rooms";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "会议室编号: " << res->getInt("id")
                << ", 名称: " << res->getString("name")
                << ", 容纳人数: " << res->getInt("capacity")
                << ", 需审批: " << (res->getBoolean("approval_required") ? "是" : "否")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "查询失败: " << e.what() << std::endl;
    }
}

void Database::clearOldReservations() {
    try {
        std::string query = "UPDATE reservations SET is_deleted = 1 WHERE date < CURDATE()";
        execute(query);
        std::cout << "已清空过期的预约。" << std::endl;
    }
    catch (sql::SQLException & e) {
        std::cout << "清空预约失败: " << e.what() << std::endl;
    }
}
