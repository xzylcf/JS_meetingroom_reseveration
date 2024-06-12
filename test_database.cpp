#include "pch.h"

// DatabaseTest �����ڲ��� Database ��
class DatabaseTest : public ::testing::Test {
protected:
    Database db;

    void SetUp() override {
        // ���ԤԼ�Ͳ����û�
        db.execute("DELETE FROM reservations WHERE 1=1");
        db.execute("DELETE FROM users WHERE username = 'testuser'");
        db.execute("DELETE FROM users WHERE username = 'adminuser'");

        // ��Ӳ����û�
        db.execute("INSERT INTO users (username, password, role_id) VALUES ('testuser', 'password', 1)");
        db.execute("INSERT INTO users (username, password, role_id) VALUES ('adminuser', 'password', 2)");

        // ��Ӳ��Ի�����

    }

    void TearDown() override {
        // ���ԤԼ�Ͳ����û�
        db.execute("DELETE FROM reservations WHERE 1=1");
        db.execute("DELETE FROM users WHERE username = 'testuser'");
        db.execute("DELETE FROM users WHERE username = 'adminuser'");
    }
};

// �����û���֤����
TEST_F(DatabaseTest, VerifyUserTest) {
    int user_id;
    EXPECT_TRUE(db.verifyUser("testuser", "password", 1, user_id));
    
    EXPECT_FALSE(db.verifyUser("testuser", "wrongpassword", 1, user_id));
    EXPECT_FALSE(db.verifyUser("wronguser", "password", 1, user_id));
}

// ������Ӻ���֤ԤԼ����
TEST_F(DatabaseTest, AddAndVerifyReservation) {
    int user_id;
    ASSERT_TRUE(db.verifyUser("testuser", "password", 1, user_id)); 
    user_id = 1;
    std::string date = "2024-06-10";
    std::string time_slot = "morning";
    int room_id = 1;


    db.execute("INSERT INTO reservations (user_id, room_id, date, time_slot, status, is_deleted) VALUES (" +
        std::to_string(user_id) + ", " + std::to_string(room_id) + ", '" + date + "', '" + time_slot + "', 'pending', 0)");

    EXPECT_FALSE(db.isRoomAvailable(room_id, date, time_slot));

    db.cancelReservation(1, user_id);
    EXPECT_FALSE(db.isRoomAvailable(room_id, date, time_slot));
}

// ����ȡ��ԤԼ����
TEST_F(DatabaseTest, CancelReservationTest) {
    int user_id;
    ASSERT_TRUE(db.verifyUser("testuser", "password", 1, user_id));

    db.execute("INSERT INTO reservations (user_id, room_id, date, time_slot, status, is_deleted) VALUES (" +
        std::to_string(user_id) + ", 1, '2024-06-10', 'morning', 'pending', 0)");

    EXPECT_FALSE(db.isUserReservation(1, user_id));
    db.cancelReservation(1, user_id);
    EXPECT_FALSE(db.isUserReservation(1, user_id));
}

// ���Է�������Թ���
TEST_F(DatabaseTest, IsRoomAvailableTest) {
    int user_id;
    ASSERT_TRUE(db.verifyUser("testuser", "password", 1, user_id));

    std::string date = "2024-06-10";
    std::string time_slot = "morning";
    int room_id = 1;
    user_id = 1;

    db.execute("INSERT INTO reservations (user_id, room_id, date, time_slot, status, is_deleted) VALUES (" +
        std::to_string(user_id) + ", " + std::to_string(room_id) + ", '" + date + "', '" + time_slot + "', 'pending', 0)");

    EXPECT_FALSE(db.isRoomAvailable(room_id, date, time_slot));

    db.cancelReservation(1, user_id);
    EXPECT_FALSE(db.isRoomAvailable(room_id, date, time_slot));
}

// ��������û�����
TEST_F(DatabaseTest, AddUserTest) {
    db.addUser("newuser213", "newpassword", 1);
    int user_id;
    EXPECT_TRUE(db.verifyUser("newuser", "newpassword", 1, user_id));
}

// �����г��û�����
TEST_F(DatabaseTest, ListUsersByRoleTest) {

    db.listUsersByRole(1);
    db.listUsersByRole(2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);



    return RUN_ALL_TESTS();
}
