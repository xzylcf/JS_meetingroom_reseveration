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

        // �����ַ���Ϊ UTF-8
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        stmt->execute("SET NAMES 'utf8mb4'");
    }
    catch (sql::SQLException & e) {
        std::cout << "���ݿ�����ʧ��: " << e.what() << std::endl;
    }
}

Database::~Database() {
    // ���ӻ��Զ��ر�
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
        std::cout << "��֤�û�ʧ��: " << e.what() << std::endl;
        return false;
    }
}

void Database::query(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
        while (res->next()) {
            std::cout << "�����ұ��: " << res->getInt("id")
                << ", ����: " << res->getString("name")
                << ", ��������: " << res->getInt("capacity")
                << ", ������: " << (res->getBoolean("approval_required") ? "��" : "��")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ѯʧ��: " << e.what() << std::endl;
    }
}

void Database::execute(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        stmt->execute(sql);
    }
    catch (sql::SQLException & e) {
        std::cout << "ִ��ʧ��: " << e.what() << std::endl;
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
        std::cout << "��鷿�������ʧ��: " << e.what() << std::endl;
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
        std::cout << "��鷿������ʧ��: " << e.what() << std::endl;
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
        std::cout << "���ԤԼ����ʧ��: " << e.what() << std::endl;
        return false;
    }
}

void Database::cancelReservation(int reservation_id, int user_id) {
    try {
        if (isUserReservation(reservation_id, user_id)) {
            std::string query = "UPDATE reservations SET is_deleted = 1 WHERE id = " + std::to_string(reservation_id);
            execute(query);
            std::cout << "ԤԼ��ȡ����" << std::endl;
        }
        else {
            std::cout << "������ȡ�������˵�ԤԼ��" << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "ȡ��ʧ��: " << e.what() << std::endl;
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
            std::string room_status = "�����ұ��: " + std::to_string(res->getInt("id")) +
                ", ����: " + res->getString("name") +
                ", ��������: " + std::to_string(res->getInt("capacity")) +
                ", ������: " + (res->getBoolean("approval_required") ? "��" : "��") +
                ", Ԥ�����: " + (res->getInt("booked") > 0 ? "��Ԥ��" : "����");
            status.push_back(room_status);
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ȡ������״̬ʧ��: " << e.what() << std::endl;
    }
    return status;
}

void Database::listUserReservations(int user_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM reservations WHERE user_id = " + std::to_string(user_id) + " AND is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "ԤԼ���: " << res->getInt("id")
                << ", �����ұ��: " << res->getInt("room_id")
                << ", ����: " << res->getString("date")
                << ", ʱ��: " << res->getString("time_slot")
                << ", ״̬: " << res->getString("status")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ѯʧ��: " << e.what() << std::endl;
    }
}

void Database::listAllReservations() {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM reservations WHERE is_deleted = 0";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "ԤԼ���: " << res->getInt("id")
                << ", �û����: " << res->getInt("user_id")
                << ", �����ұ��: " << res->getInt("room_id")
                << ", ����: " << res->getString("date")
                << ", ʱ��: " << res->getString("time_slot")
                << ", ״̬: " << res->getString("status")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ѯʧ��: " << e.what() << std::endl;
    }
}

void Database::addUser(const std::string& username, const std::string& password, int role_id) {
    try {
        std::string query = "INSERT INTO users (username, password, role_id) VALUES ('" + username + "', '" + password + "', " + std::to_string(role_id) + ")";
        execute(query);
        std::cout << "�û�����ӡ�" << std::endl;
    }
    catch (sql::SQLException & e) {
        std::cout << "����û�ʧ��: " << e.what() << std::endl;
    }
}

void Database::listUsersByRole(int role_id) {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM users WHERE role_id = " + std::to_string(role_id);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "�û����: " << res->getInt("id")
                << ", �û���: " << res->getString("username")
                << ", ��ɫ���: " << res->getInt("role_id")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ѯʧ��: " << e.what() << std::endl;
    }
}

void Database::listMeetingRooms() {
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::string query = "SELECT * FROM meeting_rooms";
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        while (res->next()) {
            std::cout << "�����ұ��: " << res->getInt("id")
                << ", ����: " << res->getString("name")
                << ", ��������: " << res->getInt("capacity")
                << ", ������: " << (res->getBoolean("approval_required") ? "��" : "��")
                << std::endl;
        }
    }
    catch (sql::SQLException & e) {
        std::cout << "��ѯʧ��: " << e.what() << std::endl;
    }
}

void Database::clearOldReservations() {
    try {
        std::string query = "UPDATE reservations SET is_deleted = 1 WHERE date < CURDATE()";
        execute(query);
        std::cout << "����չ��ڵ�ԤԼ��" << std::endl;
    }
    catch (sql::SQLException & e) {
        std::cout << "���ԤԼʧ��: " << e.what() << std::endl;
    }
}
