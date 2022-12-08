#include <iostream>
#include <string>
#include <conio.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <sstream> 
using namespace std;

//db connection
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";

typedef struct {
    double jumlah_uang;
}mesin_atm;

typedef struct {
    string nama_lengkap;
    string email;
    string tgl_lahir;
    string no_rek;
    int pin;
    int status_blokir;
    int status_kartu;
}Nasabah;

typedef struct {
    string no_rek;
    double saldo;
}Rekening;

typedef struct {
    int hh;
    int mm;
    int ss;
    int dd;
    int mn;
    int yy;
}Time;


void homePage();
void MenuOpsi1();
void halRegister(Nasabah *nsb);
void halLogin(Nasabah nsb);
void resiRegister(Nasabah nsb);

//database
void database_migration();

int main()
{
    int op;
    Nasabah nsb;

    homePage();
    MenuOpsi1();
    cout << "\nMasukkan Opsi : ";
    cin >> op;

    while (op != 99)
    {
        switch (op)
        {
        case 1:halLogin(nsb); break;
        case 2:system("cls"); halRegister(&nsb); break;
        case 123: database_migration(); break;
        default:
            cout << "Masukkan Opsi yang valid!\n";
            break;
        }
        system("pause");
        system("cls");
        homePage();
        MenuOpsi1();
        cout << "\nMasukkan Opsi : ";
        cin >> op;                                                                             
    }
}

void homePage() {
    cout << "SELAMAT DATANG DI ATM BANK NUR INDONESIA\n";
    cout << "Nikmati fitur transaksi kami selama 24 jam\n";
    cout << "Info pelayanan dan penawaran dapat di cek pada laman resmi kami di www.banknur.co.id\n";
    cout << "=====================================================================================\n\n";
}

void MenuOpsi1() {
    cout << "1 : Login\n";
    cout << "2 : Register\n";
}

void halRegister(Nasabah *nsb) {
    int pin;

    cout << "Register\n";
    cout << "=====================================================================================\n\n";

    cout << "Silahkan masukkan Nama Lengkap anda : ";
    cin >> nsb->nama_lengkap;
    
    cout << "\nSilahkan masukkan E-Mail anda : ";
    cin >> nsb->email;
    
    cout << "\nSilahkan masukkan PIN (8 digit) anda : ";
    cin >> pin;
    stringstream ss;
    ss << pin;
    string pins;
    ss >> pins;

    if (strlen(pins.c_str()) == 8)
    {
        nsb->pin = pin;
    }

    while (strlen(pins.c_str()) != 8)
    {
        cout << "PIN tidak boleh lebih/kurang dari 8 digit!!\n";
        exit(1);
    }

    cout << "\nMasukkan Tanggal Lahir (YYYY-MM-DD) : ";
    cin >> nsb->tgl_lahir;

    srand(time(0));
    int random = 100000000 + (rand() % 999999999);
    stringstream nr;
    nr << random;
    string no_rek;
    nr >> no_rek;

    nsb->no_rek = no_rek;

    system("cls");
    resiRegister(*nsb);
}

void resiRegister(Nasabah nsb) {
    if (nsb.email != "" && nsb.nama_lengkap != "" && nsb.pin != NULL && nsb.tgl_lahir != "") {
        cout << "SELAMAT AKUN ANDA BERHASIL DIDAFTARKAN\n";
        cout << "Nama Lengkap : " << nsb.nama_lengkap << "\n";
        cout << "E-Mail : " << nsb.email << "\n";
        cout << "Tanggal Lahir : " << nsb.tgl_lahir << "\n";
        cout << "No. Rekening : " << nsb.no_rek << "\n\n";
    }
    else {
        cout << "MOHON MAAF, AKUN ANDA GAGAL DIDAFTARKAN\n";
    }
}



void halLogin(Nasabah nsb) {
        
}

void database_migration() {
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;

    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    //please create database "quickstartdb" ahead of time
    con->setSchema("banknur");

    stmt = con->createStatement();
    stmt->execute("CREATE TABLE nasabah (id serial PRIMARY KEY, nama_lengkap VARCHAR(255), email VARCHAR(255), tgl_lahir DATE, no_rek VARCHAR(9), pin INTEGER(8), status_blokir INTEGER(1) DEFAULT 0, status_kartu INTEGER(1) DEFAULT 0);");
    cout << "Finished creating table 1" << endl;
    
    stmt->execute("CREATE TABLE rekening (id serial PRIMARY KEY, no_rek VARCHAR(9), saldo DOUBLE);");
    cout << "Finished creating table 2" << endl;
    
    stmt->execute("CREATE TABLE data_bank (id serial PRIMARY KEY, kode_bank VARCHAR(3), nama_bank VARCHAR(255));");
    cout << "Finished creating table 2" << endl;
    
    stmt->execute("CREATE TABLE transaksi (id serial PRIMARY KEY, no_rek VARCHAR(9), jenis_transaksi VARCHAR(255), timestamp DATETIME);");
    cout << "Finished creating table 2" << endl;

    delete stmt;
    delete con;
}