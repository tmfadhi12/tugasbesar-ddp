#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <sstream> 
#include <ctime> 
#include <windows.h>
using namespace std;

//db connection
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";

typedef struct {
	string nama_lengkap;
	string email;
	string tgl_lahir;
	string no_rek;
	string pin;
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

static void resiRegister(Nasabah nsb);
static void sendtodb(Nasabah nsb);
static void emailverif(string email);
static void pinverif(string eamil, string pin);
static void blokirkartu(string email);
static void getSaldofromDB(string email);
static void systemTarikTunai(int nominal, string email);
static void systemTarikTunaiCalc(string email, string nominal, int jumlahtariktunai);
static void systemSetorTunai(int nominal, string email);
static void systemSetorTunaiCalc(string email, string nominal, int jumlahtariktunai);
static void resiPenarikanBerhasil(int jumlahpenarikan);
static void createHistoryTransaksi(string no_rek, string nama_transaksi, int nominal);
static void resiSetoranBerhasil(int jumlahpenarikan);
static void getDaftarMutasi(string email, int jumlahitem);
static void validasiNoRekAntarRek(string email, string no_rekTujuan);
static void systemTransaksiAntarRek(string email, string no_rekTujuan, int nominaltf);
static void konfirmasiTransfer(string no_rekTujuan, int nominaltf);
static void resiTf(string nama_penerima, int nominaltf, string norek);
static void validasiTglLahir(string tgl_lahir, string email);
static void systemUbahPIN(string email, int PIN);
static void systemUbahEmail(string email, string konfirmasiemail);
static void systemStatAktivasi(string email, int opsi);
static void validasiStatusKartu(string email);

static bool login_stats = 0;
static bool email_verif = 0;
static int	wrongpincounter = 0;
static int	blokir_stats;
static int stat_exit = 1;
static bool tgl_lahir_statubahpin = 0;
static bool tgl_lahir_statubahemail = 0;
static bool tgl_lahir_statusubahaktivasi = 0;
static string statuskartu;

static void resiRegister(Nasabah nsb) {
	if (nsb.email != "" && nsb.nama_lengkap != "" && nsb.pin != "" && nsb.tgl_lahir != "") {
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

static void sendtodb(Nasabah nsb) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	time_t now = time(0);
	string tgl = ctime(&now);

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("INSERT INTO nasabah(nama_lengkap,email,tgl_lahir,no_rek,pin) VALUES(?,?,?,?,?)");
	pstmt->setString(1, nsb.nama_lengkap);
	pstmt->setString(2, nsb.email);
	pstmt->setString(3, nsb.tgl_lahir);
	pstmt->setString(4, nsb.no_rek);
	pstmt->setString(5, nsb.pin);
	pstmt->execute();

	pstmt = con->prepareStatement("INSERT INTO rekening(no_rek,saldo) VALUES(?,?)");
	pstmt->setString(1, nsb.no_rek);
	pstmt->setBigInt(2, "500000");
	pstmt->execute();

	pstmt = con->prepareStatement("INSERT INTO transaksi(no_rek,jenis_transaksi,nominal,timestamp) VALUES(?,?,?,?)");
	pstmt->setString(1, nsb.no_rek);
	pstmt->setString(2, "Pembukaan Tabungan");
	pstmt->setString(3, "500000");
	pstmt->setString(4, tgl);
	pstmt->execute();

	resiRegister(nsb);

	delete pstmt;
	delete con;
}

static void emailverif(string email) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string sv_email;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT * FROM `nasabah` WHERE email='" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		sv_email = result->getString(3);
		blokir_stats = result->getInt(7);
		goto email_verification;
	}


	delete result;
	delete pstmt;
	delete con;

email_verification:
	system("cls");
	if (sv_email == "")
	{
		cout << "Login\n";
		cout << "=====================================================================================\n\n";
		cout << "Akun tidak terdaftar, silahkan menuju halaman Register untuk melakukan pendaftaran\n";
		system("pause");
	}
	else {
		email_verif = 1;
	}
}

static void pinverif(string email, string pin) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	int sv_statusblokir;
	string sv_pin;
	string sv_email;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT * FROM `nasabah` WHERE email='" + email + "'AND pin='" + pin + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		sv_statusblokir = result->getInt(7);
		sv_pin = result->getString(6);
		sv_email = result->getString(3);
		goto pin_verification;
	}


	delete result;
	delete pstmt;
	delete con;

pin_verification:
	system("cls");
	if (sv_pin == "")
	{
		wrongpincounter++;
		cout << "Login\n";
		cout << "=====================================================================================\n\n";
		cout << "Pin Salah! Mohon cek lebih teliti lagi. \n";
		system("pause");
	}
	else {
		login_stats = 1;
	}
}

static void blokirkartu(string email) {
	sql::Driver* driver;
	sql::Connection* con;
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

	con->setSchema("banknur");
	pstmt = con->prepareStatement("UPDATE nasabah SET status_blokir = 1 WHERE email ='" + email + "'");
	pstmt->executeQuery();
	cout << "Login\n";
	cout << "=====================================================================================\n\n";
	cout << "Mohon maaf, Demi keamanan akun anda diblokir dikarenakan melakukan kesalahan dalam memasukkan PIN sebanyak 3x.\n";
	cout << "Silahkan datang ke kantor pusat/cabang upaya melakukan pembebasan status blokir kartu anda, terima kasih.\n";

	delete pstmt;
	delete con;
	system("pause");
	exit(1);
}

static void getSaldofromDB(string email) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	static long int saldo;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.email FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.email = '"+email+"'");
	result = pstmt->executeQuery();

	while (result->next()) {
		cout << "\n" << "Rp. " << result->getInt(2) << "\n\n";
	}

	delete result;
	delete pstmt;
	delete con;
}

static void systemTarikTunai(int nominal, string email) {

	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	static long int saldo;
	static long int hasil;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.email FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		saldo =  result->getInt(2);
	}

	delete result;
	delete pstmt;
	delete con;

	if (saldo < nominal)
	{
		int ops;
	input:
		system("cls");
		cout << "Tarik Tunai\n";
		cout << "=====================================================================================\n";
		cout << "Mohon Maaf, Saldo anda tidak cukup untuk melakukan penarikan tunai\n\n";
		cout << "98.Kembali\n";
		cout << "99.Keluar\n\n";

		cout << "Masukkan Opsi : ";
		cin >> ops;

		if (cin.fail())
		{
			cout << "Masukkan Input yang Valid!";
			cin.clear();
			cin.ignore();
		}
		
		if (ops == 98)
		{
			system("cls");
			stat_exit = 0;
		}
		else if(ops == 99){
			exit(1);
		}
		else {
			cout << "Masukkan Input yang Valid!";
			system("pause");
			goto input;
		}
	}
	else {
		hasil = saldo - nominal;
		stringstream sldtostr;
		sldtostr << hasil;
		string saldostr;
		sldtostr >> saldostr;

		systemTarikTunaiCalc(email, saldostr,nominal);
	}
}

static void systemTarikTunaiCalc(string email, string nominal, int jumlahtariktunai) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;
	int opt;
	string no_rek;

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

	con->setSchema("banknur");
	pstmt = con->prepareStatement("UPDATE rekening AS rek JOIN (SELECT nasabah.email, nasabah.no_rek FROM nasabah GROUP BY no_rek HAVING nasabah.email='"+email+"')ns ON rek.no_rek=ns.no_rek SET rek.saldo='"+nominal+"'");
	pstmt->executeQuery();

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.email FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		no_rek = result->getString(1);
	}

	
	createHistoryTransaksi(no_rek, "Tarik Tunai", jumlahtariktunai);
	resiPenarikanBerhasil(jumlahtariktunai);

	delete pstmt;
	delete con;
	delete result;

	cout << "Lanjut bertransaksi ?\n";
	cout << "1. Ya\n";
	cout << "2. Tidak\n\n";
	cout << "Masukkan Opsi : ";
	cin >> opt;

	switch (opt)
	{
	case 1: stat_exit = 0; break;
	case 2: exit(1); break;
	default:
		cout << "Masukkan input yang valid!";
		if (cin.fail())
		{
			cin.clear();
			cin.ignore();
		}
		break;
	}
}

static void resiPenarikanBerhasil(int jumlahpenarikan) {
	string day;
	time_t now = time(0);
	tm *ltm = localtime(&now);

	switch (ltm->tm_wday)
	{
	case 0:day="Senin"; break;
	case 1:day="Selasa"; break;
	case 2:day="Rabu"; break;
	case 3:day="Kamis"; break;
	case 4:day="Jumat"; break;
	case 5:day="Sabtu"; break;
	case 6:day="Minggu"; break;
	default:
		break;
	}

	system("cls");
	cout << "Tarik Tunai\n";
	cout << "=====================================================================================\n\n";
	cout << "Penarikan Berhasil.\n";
	cout << "Terima Kasih telah menggunakan layanan Bank Kami.\n\n";
	cout << "--------------------------------------\n\n";
	cout << "ATM BNI\n\n";
	cout << "Jumlah Uang yang di Tarik\n\n";
	cout << "Rp. "<<jumlahpenarikan << "\n";
	cout << day << " " << ltm->tm_mday << "-" << ltm->tm_mon+1 << "-" << 1900+ltm->tm_year <<  " " << ltm->tm_hour+5 << ":" << ltm->tm_min+30 << ":" << ltm->tm_sec << "\n\n";
	cout << "Info lebih lanjut Hubungi Kami di 12909\n";
	cout << "--------------------------------------\n\n";
}

static void systemSetorTunai(int nominal, string email) {

	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	static long int saldo;
	static long int hasil;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.email FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		saldo = result->getInt(2);
	}

	delete result;
	delete pstmt;
	delete con;

	if (nominal >= 10000000)
	{
	input:
		int ops;
		system("cls");
		cout << "Setor Tunai";
		cout << "=====================================================================================\n";
		cout << "Mohon Maaf, Limit Setoran Tunai Adalah Rp. 10,000,000\n\n";

		cout << "98.Kembali\n";
		cout << "99.Keluar\n\n";
		cout << "Masukkan Opsi : ";
		cin >> ops;
		
		if (ops == 99)
		{
			exit(1);
		}
		else if (ops == 98) {
			system("cls");
			stat_exit = 0;
		}
		else {
			cout << "Masukkan Input yang Valid!";
			system("pause");
			if (cin.fail())
			{
				cin.clear();
				cin.ignore();
			}
			goto input;
		}
	}
	else {
		hasil = saldo + nominal;
		stringstream sldtostr;
		sldtostr << hasil;
		string saldostr;
		sldtostr >> saldostr;

		systemSetorTunaiCalc(email, saldostr, nominal);
	}
}

static void systemSetorTunaiCalc(string email, string nominal, int jumlahtariktunai) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;
	int opt;
	string no_rek;

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

	con->setSchema("banknur");
	pstmt = con->prepareStatement("UPDATE rekening AS rek JOIN (SELECT nasabah.email, nasabah.no_rek FROM nasabah GROUP BY no_rek HAVING nasabah.email='" + email + "')ns ON rek.no_rek=ns.no_rek SET rek.saldo='" + nominal + "'");
	pstmt->executeQuery();

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.email FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		no_rek = result->getString(1);
	}


	createHistoryTransaksi(no_rek, "Setor Tunai", jumlahtariktunai);
	resiSetoranBerhasil(jumlahtariktunai);

	delete pstmt;
	delete con;
	delete result;

	cout << "Lanjut bertransaksi ?\n";
	cout << "1. Ya\n";
	cout << "2. Tidak\n\n";
	cout << "Masukkan Opsi : ";
	cin >> opt;

	switch (opt)
	{
	case 1: stat_exit = 0; break;
	case 2: exit(1); break;
	default:
		cout << "Masukkan input yang valid!";
		if (cin.fail())
		{
			cin.clear();
			cin.ignore();
		}
		break;
	}
}

static void resiSetoranBerhasil(int jumlahpenarikan) {
	string day;
	time_t now = time(0);
	tm* ltm = localtime(&now);

	switch (ltm->tm_wday)
	{
	case 0:day = "Senin"; break;
	case 1:day = "Selasa"; break;
	case 2:day = "Rabu"; break;
	case 3:day = "Kamis"; break;
	case 4:day = "Jumat"; break;
	case 5:day = "Sabtu"; break;
	case 6:day = "Minggu"; break;
	default:
		break;
	}

	system("cls");
	cout << "Setor Tunai\n";
	cout << "=====================================================================================\n\n";
	cout << "Setoran Berhasil.\n";
	cout << "Terima Kasih telah menggunakan layanan Bank Kami.\n\n";
	cout << "--------------------------------------\n\n";
	cout << "ATM BNI\n\n";
	cout << "Jumlah Uang yang di Setorkan\n\n";
	cout << "Rp. " << jumlahpenarikan << "\n";
	cout << day << " " << ltm->tm_mday << "-" << ltm->tm_mon + 1 << "-" << 1900 + ltm->tm_year << " " << ltm->tm_hour + 5 << ":" << ltm->tm_min + 30 << ":" << ltm->tm_sec << "\n\n";
	cout << "Info lebih lanjut Hubungi Kami di 12909\n";
	cout << "--------------------------------------\n\n";
}

static void createHistoryTransaksi(string no_rek, string nama_transaksi, int nominal) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	stringstream nominaltostr;
	nominaltostr << nominal;
	string nominalstr;
	nominaltostr >> nominalstr;

	time_t now = time(0);
	string tgl = ctime(&now);

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("INSERT INTO transaksi(no_rek,jenis_transaksi,nominal,timestamp) VALUES(?,?,?,?)");
	pstmt->setString(1, no_rek);
	pstmt->setString(2, nama_transaksi);
	pstmt->setString(3, nominalstr);
	pstmt->setString(4, tgl);
	pstmt->execute();

	delete pstmt;
	delete con;
}

static void getDaftarMutasi(string email, int jumlahitem) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	stringstream jmltostr;
	jmltostr << jumlahitem;
	string jmlstr;
	jmltostr >> jmlstr;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT transaksi.no_rek, nasabah.email, transaksi.timestamp,transaksi.jenis_transaksi,transaksi.nominal, transaksi.detail FROM nasabah INNER JOIN transaksi ON nasabah.no_rek = transaksi.no_rek HAVING nasabah.email ='"+email+"' ORDER BY transaksi.id DESC LIMIT "+jmlstr+"");
	result = pstmt->executeQuery();

	while (result->next()) {
		cout << result->getString(4) << " Rp." << result->getString(5) << "\n";
		if (result->getString(6)!="")
		{
			cout << result->getString(6) << "\n";
		}
		cout << result->getString(3) << "\n\n";
	}

	/*displayMutasi(timestamp, jenis_transaksi, nominal);*/

	delete result;
	delete pstmt;
	delete con;
}

static void validasiNoRekAntarRek(string email,string no_rekTujuan) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string no_rektujuan;

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

	con->setSchema("banknur");
	
	pstmt = con->prepareStatement("SELECT nasabah.no_rek FROM nasabah WHERE nasabah.no_rek = '" + no_rekTujuan + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		no_rektujuan = result->getString(1);
	}

	delete result;
	delete pstmt;
	delete con;

	if (no_rektujuan == "")
	{
		system("cls");
		cout << "Transfer Antar Rekening\n";
		cout << "=====================================================================================\n";
		cout << "Kode Rekening Bank Nur Indonesia : 221524XXX\n\n";

		cout << "Mohon Maaf, Tidak ada Nasabah yang di Maksud.\n\n";
	}
	else {
		int nominaltf;
		system("cls");
input:
		cout << "Transfer Antar Rekening\n";
		cout << "=====================================================================================\n";

		cout << "Masukkan Nominal Transfer : Rp. ";
		cin >> nominaltf;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore();
		}

		while (nominaltf <= 10000 || nominaltf >= 50000000)
		{
			cout << "Nominal Minimum Transfer Adalah Rp. 10,000 dan Maksimum Transfer Adalah Rp. 50,000,000\n\n";
			system("pause");
			system("cls");
			goto input;
		}
		
		system("cls");
		systemTransaksiAntarRek(email,no_rekTujuan,nominaltf);
	}
}

static void systemTransaksiAntarRek(string email,string no_rekTujuan, int nominaltf) {

	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	static int saldo_asal;
	static int saldo_tujuan;
	string rek_asal;
	string nama_pengirim;
	string nama_penerima;

	time_t now = time(0);
	string tgl = ctime(&now);

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.nama_lengkap FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek WHERE nasabah.email = '"+email+"'");
	result = pstmt->executeQuery();

	while (result->next()) {
		saldo_asal = result->getInt(2);
		rek_asal = result->getString(1);
		nama_pengirim = result->getString(3);
	}

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.nama_lengkap FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.no_rek = '"+no_rekTujuan+"'");
	result = pstmt->executeQuery();

	while (result->next()) {
		saldo_tujuan = result->getInt(2);
		nama_penerima = result->getString(3);
	}

	if (saldo_asal < nominaltf)
	{
		int opsi;
		input:
		cout << "Transfer Antar Rekening\n";
		cout << "=====================================================================================\n\n";
		cout << "Mohon Maaf, Saldo yang anda miliki tidak cukup untuk melanjutkan transaksi.\n\n";

		cout << "98.Home\n";
		cout << "99.Cancel\n\n";

		cout << "Masukkan Opsi : ";
		cin >> opsi;
		if (opsi == 99) {
			exit(1);
		}else if(opsi == 98)
		{
			stat_exit = 0;
		}
		else {
			cout << "Masukkan Input yang Valid!";
			system("pause");
			if (cin.fail())
			{
				cin.clear();
				cin.ignore();
			}
			system("cls");
			goto input;
		}
	}

	if (stat_exit==1)
	{
		konfirmasiTransfer(no_rekTujuan, nominaltf);

		int hasilSaldoPengirim = saldo_asal - nominaltf;
		stringstream inttostr;
		inttostr << hasilSaldoPengirim;
		string saldopengirim;
		inttostr >> saldopengirim;

		int hasilSaldoPenerima = saldo_tujuan + nominaltf;
		stringstream inttostr2;
		inttostr2 << hasilSaldoPenerima;
		string saldopenerima;
		inttostr2 >> saldopenerima;

		con->setSchema("banknur");
		pstmt = con->prepareStatement("UPDATE rekening AS rek JOIN (SELECT nasabah.email, nasabah.no_rek FROM nasabah GROUP BY no_rek HAVING nasabah.email='" + email + "')ns ON rek.no_rek=ns.no_rek SET rek.saldo='" + saldopengirim + "'");
		pstmt->executeQuery();

		pstmt = con->prepareStatement("INSERT INTO transaksi(no_rek,jenis_transaksi,nominal,detail,timestamp) VALUES(?,?,?,?,?)");
		pstmt->setString(1, rek_asal);
		pstmt->setString(2, "Transfer");
		pstmt->setInt(3, nominaltf);
		pstmt->setString(4, nama_penerima);
		pstmt->setString(5, tgl);
		pstmt->execute();

		con->setSchema("banknur");
		pstmt = con->prepareStatement("UPDATE rekening AS rek JOIN (SELECT nasabah.email, nasabah.no_rek FROM nasabah GROUP BY no_rek HAVING nasabah.no_rek='" + no_rekTujuan + "')ns ON rek.no_rek=ns.no_rek SET rek.saldo='" + saldopenerima + "'");
		pstmt->executeQuery();

		pstmt = con->prepareStatement("INSERT INTO transaksi(no_rek,jenis_transaksi,nominal,detail,timestamp) VALUES(?,?,?,?,?)");
		pstmt->setString(1, no_rekTujuan);
		pstmt->setString(2, "Transaksi Masuk");
		pstmt->setInt(3, nominaltf);
		pstmt->setString(4, nama_pengirim);
		pstmt->setString(5, tgl);
		pstmt->execute();

		delete result;
		delete pstmt;
		delete con;

		resiTf(nama_penerima, nominaltf, no_rekTujuan);
	}
}

static void resiTf(string nama_penerima,int nominaltf, string norek) {
	system("cls");
	cout << "Transfer Antar Rekening\n";
	cout << "=====================================================================================\n";
	cout << "Transfer Berhasil\n\n";
	
	cout << "-------------------------------------\n";
	cout << "---------------ATM BNI---------------\n";
	cout << "Nama Penerima : " << nama_penerima << "\n";
	cout << "No. Rekening Penerima : " << norek << "\n";
	cout << "Nama BANK : Bank Nur Indonesia (BNI)\n";
	cout << "Jumlah Transfer : Rp. " << nominaltf << "\n";
	cout << "Biaya Admin : Rp. 0\n";
	cout << "-------------------------------------\n\n";
}

static void konfirmasiTransfer(string no_rekTujuan, int nominaltf) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string nama_penerima;
	int ops;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, rekening.saldo, nasabah.nama_lengkap FROM nasabah INNER JOIN rekening ON nasabah.no_rek = rekening.no_rek HAVING nasabah.no_rek = '" + no_rekTujuan + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		nama_penerima = result->getString(3);
	}

	cout << "Transfer Antar Rekening\n";
	cout << "=====================================================================================\n";
	cout << "Konfirmasi Transaksi\n\n";

	cout << "Nasabah Penerima : " << nama_penerima << "\n";
	cout << "Nama Bank : Bank Nur Indonesia (BNI)\n";
	cout << "Nominal Transfer : Rp. " << nominaltf << "\n";
	cout << "Biaya Admin : Rp. 0\n\n";

	cout << "Apakah Anda Yakin akan Melanjutkan Transaksi ?\n";
	cout << "1.Ya\n\n";

	cout << "98.Kembali\n";
	cout << "99.Keluar\n";

	cout << "Masukkan Opsi : ";
	cin >> ops;

	while (ops!=1)
	{
		switch (ops)
		{
		case 98: stat_exit = 0; break;
		case 99: exit(1); break;
		default:
			cout << "Masukkan input yang valid!";
			if (cin.fail())
			{
				cin.clear();
				cin.ignore();
			}
			break;
		}
		system("pause");
		system("cls");
		cout << "Transfer Antar Rekening\n";
		cout << "=====================================================================================\n";
		cout << "Konfirmasi Transaksi\n\n";

		cout << "Nasabah Penerima : " << nama_penerima << "\n";
		cout << "Nama Bank : Bank Nur Indonesia (BNI)\n";
		cout << "Nominal Transfer : Rp. " << nominaltf << "\n";
		cout << "Biaya Admin : Rp. 0\n\n";

		cout << "Apakah Anda Yakin akan Melanjutkan Transaksi ?\n";
		cout << "1.Ya\n\n";

		cout << "98.Kembali\n";
		cout << "99.Keluar\n";

		cout << "Masukkan Opsi : ";
		cin >> ops;
	}
}

static void validasiTglLahir(string tgl_lahir, string email) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string nama_penerima;
	int ops;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.no_rek, nasabah.nama_lengkap, nasabah.email FROM nasabah WHERE nasabah.email = '"+email+"' AND nasabah.tgl_lahir='"+tgl_lahir+"'");
	result = pstmt->executeQuery();

	while (result->next()) {
		nama_penerima = result->getString(2);
	}

	delete result;
	delete pstmt;
	delete con;

	if (nama_penerima == "")
	{
		system("cls");
		cout << "Login\n";
		cout << "=====================================================================================\n\n";
		cout << "Tanggal Lahir yang Anda Masukkan Salah! Mohon cek lebih teliti lagi. \n";
		system("pause");
	}
	else {
		tgl_lahir_statubahpin = 1;
		tgl_lahir_statubahemail = 1;
		tgl_lahir_statusubahaktivasi = 1;
	}
}

static void systemUbahPIN(string email, int PIN) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	stringstream inttostr;
	inttostr << PIN;
	string pinstr;
	inttostr >> pinstr;

	string pin_lama;
	int ops;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.pin, nasabah.nama_lengkap, nasabah.email FROM nasabah WHERE nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		pin_lama = result->getString(1);
	}

	if (pinstr == pin_lama)
	{
		cout << "Tidak Dapat Menggunakan PIN Lama Anda! Ubah menjadi PIN baru atau Batalkan Perubahan\n";
		system("pause");
	}
	else {
		pstmt = con->prepareStatement("UPDATE nasabah SET pin='"+pinstr+"' WHERE email='"+email+"'");
		pstmt->executeQuery();

		cout << "PIN Berhasil di Ubah.\n\n";
		system("pause");
	}

	delete result;
	delete pstmt;
	delete con;
}

static void systemUbahEmail(string email, string konfirmasiemail) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string emaillama;
	int ops;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.pin, nasabah.nama_lengkap, nasabah.email FROM nasabah WHERE nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		emaillama = result->getString(3);
	}

	if (konfirmasiemail == emaillama)
	{
		cout << "Tidak Dapat Menggunakan E-Mail Lama Anda! Ubah menjadi E-Mail baru atau Batalkan Perubahan\n";
		system("pause");
	}
	else {
		pstmt = con->prepareStatement("UPDATE nasabah SET email='" + konfirmasiemail + "' WHERE email='" + email + "'");
		pstmt->executeQuery();

		cout << "E-Mail Berhasil di Ubah.\n\n";
		system("pause");
	}

	delete result;
	delete pstmt;
	delete con;
}

static void systemStatAktivasi(string email,int opsi) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string statkartu;
	int ops;

	stringstream inttostr;
	inttostr << opsi;
	string opsistr;
	inttostr >> opsistr;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.pin, nasabah.nama_lengkap, nasabah.status_kartu, nasabah.email FROM nasabah WHERE nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		statkartu = result->getString(3);
	}

	if (opsistr == statkartu)
	{
		cout << "Akun telah aktif/non-aktif, Pilih Opsi lain atau batalkan!\n";
		system("pause");
	}
	else {
		pstmt = con->prepareStatement("UPDATE nasabah SET status_kartu='" + opsistr + "' WHERE email='" + email + "'");
		pstmt->executeQuery();

		cout << "Status Aktivasi Berhasil di Ubah.\n\n";
		system("pause");
	}

	delete result;
	delete pstmt;
	delete con;
}

static void validasiStatusKartu(string email) {
	sql::Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

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

	con->setSchema("banknur");

	pstmt = con->prepareStatement("SELECT nasabah.pin, nasabah.nama_lengkap, nasabah.status_kartu, nasabah.email FROM nasabah WHERE nasabah.email = '" + email + "'");
	result = pstmt->executeQuery();

	while (result->next()) {
		statuskartu = result->getString(3);
	}

	if (statuskartu == "1")
	{
		cout << "Transaksi Gagal\n";
		cout << "=====================================================================================\n\n";
		cout << "Anda tidak dapat melakukan Transaksi ini karena Kartu anda telah dinonaktifkan.\n";
		cout << "Kunjungi banknur.co.id untuk Info lebih lanjut.\n\n";
		system("pause");
	}

	delete result;
	delete pstmt;
	delete con;
}