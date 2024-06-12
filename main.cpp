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
    std::cout << "�˺ţ�";
    std::cin >> username;
    std::cout << "���룺";
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
        std::cout << "��֤ʧ�ܣ������ԡ�" << std::endl;
    }
}

void developerMenu(Database& db, int user_id) {
    int choice;
    do {
        std::cout << "1. ����ԤԼ\n";
        std::cout << "2. �鿴�Լ�ԤԼ\n";
        std::cout << "3. ȡ��ԤԼ\n";
        std::cout << "4. �˳�\n";
        std::cout << "ѡ��: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            db.listMeetingRooms();
            std::string date, time_slot;
            std::cout << "����ԤԼ���� (YYYY-MM-DD): ";
            std::cin >> date;
            std::cout << "����ʱ��� (morning/afternoon): ";
            std::cin >> time_slot;

            std::vector<std::string> status = db.getRoomStatus(date, time_slot);
            for (const auto& room_status : status) {
                std::cout << room_status << std::endl;
            }

            int room_id;
            std::cout << "��������ұ��: ";
            std::cin >> room_id;
            if (!db.isRoomAvailable(room_id, date, time_slot)) {
                std::cout << "ԤԼʧ�ܣ��������ѱ�ռ�á�" << std::endl;
            }
            else {
                bool approval_required = db.isSpecialRoom(room_id);
                std::string status = approval_required ? "pending" : "approved";
                std::string sql = "INSERT INTO reservations (user_id, room_id, date, time_slot, status, is_deleted) VALUES (" +
                    std::to_string(user_id) + ", " + std::to_string(room_id) + ", '" + date + "', '" + time_slot + "', '" + status + "', 0)";
                db.execute(sql);
                if (approval_required) {
                    std::cout << "ԤԼ�ɹ����ȴ�������" << std::endl;
                }
                else {
                    std::cout << "ԤԼ�ɹ���" << std::endl;
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
            std::cout << "����Ҫȡ����ԤԼ���: ";
            std::cin >> reservation_id;
            db.cancelReservation(reservation_id, user_id);
            break;
        }
        case 4: {
            std::cout << "�˳���¼" << std::endl;
            break;
        }
        default: {
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
        }
    } while (choice != 4);
}

void adminMenu(Database& db) {
    int choice;
    do {
        std::cout << "1. �鿴���л�����ԤԼ��Ϣ\n";
        std::cout << "2. ����ԤԼ\n";
        std::cout << "3. �˳�\n";
        std::cout << "ѡ��: ";
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
            std::cout << "����ԤԼ���: ";
            std::cin >> reservation_id;
            std::cout << "����������� (1: ͨ��, 0: ��ͨ��): ";
            std::cin >> approve;
            std::string status = ((approve==1) ? "approved" : "rejected");
            std::string sql = "UPDATE reservations SET status = '" + status + "' WHERE id = " + std::to_string(reservation_id);
            db.execute(sql);
            std::cout << "������ɡ�" << std::endl;
            break;
        }
        case 3: {
            std::cout << "�˳���¼" << std::endl;
            break;
        }
        default: {
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
        }
    } while (choice != 3);
}

void sysadminMenu(Database& db) {
    int choice;
    do {
        std::cout << "1. ����˻�\n";
        std::cout << "2. �鿴�˻�\n";
        std::cout << "3. �鿴��������Ϣ\n";
        std::cout << "4. ���ԤԼ\n";
        std::cout << "5. �˳�\n";
        std::cout << "ѡ��: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::string username, password;
            int role_id;
            std::cout << "�����û���: ";
            std::cin >> username;
            std::cout << "��������: ";
            std::cin >> password;
            std::cout << "ѡ���ɫ (1: �з���Ա, 2: ������Ա): ";
            std::cin >> role_id;
            db.addUser(username, password, role_id);
            break;
        }
        case 2: {
            int role_id;
            std::cout << "ѡ���ɫ (1: �з���Ա, 2: ������Ա): ";
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
            std::cout << "�˳���¼" << std::endl;
            break;
        }
        default: {
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
        }
    } while (choice != 5);
}

int main() {
    Database db;
    int choice;
    do {
        std::cout << "1. �з���Ա��¼\n";
        std::cout << "2. ������Ա��¼\n";
        std::cout << "3. ϵͳ����Ա��¼\n";
        std::cout << "4. �˳�\n";
        std::cout << "ѡ�����: ";
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
            std::cout << "�˳�ϵͳ" << std::endl;
            break;
        }
        default: {
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
        }
    } while (choice != 4);

    return 0;
}
