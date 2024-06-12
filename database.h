#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <vector>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/connection.h>

class Database {
public:
    Database();
    ~Database();
    bool verifyUser(const std::string& username, const std::string& password, int role_id, int& user_id);
    void query(const std::string& sql);
    void execute(const std::string& sql);
    bool isRoomAvailable(int room_id, const std::string& date, const std::string& time_slot);
    void listUserReservations(int user_id);
    void listAllReservations();
    void cancelReservation(int reservation_id, int user_id);
    void addUser(const std::string& username, const std::string& password, int role_id);
    void listUsersByRole(int role_id);
    void listMeetingRooms();
    void clearOldReservations();
    bool isSpecialRoom(int room_id);
    bool isUserReservation(int reservation_id, int user_id);
    std::vector<std::string> getRoomStatus(const std::string& date, const std::string& time_slot);

private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> connection;
};

#endif
