#include "database.h"

void homePage();
void MenuOpsi1();
void halRegister(Nasabah *nsb);
void halLogin();
void opsiLandingPage();
void halCekSaldo();
void halTarikTunai();
void halSetorTunai();
void halTransfer();
void halDaftarMutasi();
void halPelayanan();
void halUtama();
void antarRekening();
void antarBank();
void virtualAcc();
void ubahPIN();
void ubahEmail();
void ubahAktivasi();

string email;

int main()
{
    int op;
    Nasabah nsb;

    homePage();
    MenuOpsi1();
    cout << "\nMasukkan Opsi : ";
    cin >> op;

    while(op != 99)
    {
        
        switch (op)
        {
        case 1:system("cls"); halLogin(); break;
        case 2:system("cls"); halRegister(&nsb); break;
        default:
            cout << "Masukkan Opsi yang valid!\n";
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }
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
    cout << "2 : Register\n\n";
    cout << "99 : Keluar\n";
}

void halRegister(Nasabah *nsb) {
    string pin;

    cout << "Register\n";
    cout << "=====================================================================================\n\n";

    cin.ignore();
    cout << "Silahkan masukkan Nama Lengkap anda : ";
    getline(cin, nsb->nama_lengkap);
    
    cout << "\nSilahkan masukkan E-Mail anda : ";
    cin >> nsb->email;
 
pin_reg:
    cout << "\nSilahkan masukkan PIN (6 digit) anda : ";
    cin >> pin;

    while(strlen(pin.c_str()) != 6) {
        cout << "\nPIN tidak boleh lebih/kurang dari 6 digit!!\n";
        system("pause");
        goto pin_reg;
    }

    nsb->pin = pin;

    cout << "\nMasukkan Tanggal Lahir (DD/MM/YYYY) : ";
    cin >> nsb->tgl_lahir;

    srand(time(0));
    int random = 0 + (rand() % 999);
    stringstream nr;
    nr << random;
    string no_rek;
    nr >> no_rek;

    nsb->no_rek = "221524"+no_rek;

    system("cls");
    sendtodb(*nsb);
}

void halLogin() {
    string pin;

input_email:
    cout << "Login\n";
    cout << "=====================================================================================\n\n";

    cout << "99.Cancel\n\n";
    cout << "Silahkan masukkan E-Mail anda : ";
    cin >> email;

    if (email == "99")
    {
        exit(1);
    }
    
    emailverif(email);
    if (blokir_stats == 1)
    {
        cout << "Login\n";
        cout << "=====================================================================================\n\n";
        cout << "Akun ini telah diblokir. Info lebih lanjut datangi Bank Pusat/Cabang terdekat Kami. Terimakasih\n";
        exit(1);
    }

    if (email_verif == 1)
    {
    input_pin:
        if (login_stats == 0)
        {
            cout << "Login\n";
            cout << "=====================================================================================\n\n";

            cout << "99.Cancel\n\n";
            cout << "Silahkan masukkan PIN anda : ";
            cin >> pin;


            if (pin == "99")
            {
                exit(1);
            }

            pinverif(email, pin);
            if (wrongpincounter == 3)
            {
                system("cls");
                blokirkartu(email);
            }
        }
        else {
            system("cls");
            halUtama();
        }
        system("cls");
        goto input_pin;
    }
    else {
        system("cls");
        goto input_email;
    }
}

void halUtama(){
        int opt;

        cout << "Pilih Transaksi Pembayaran\n";
        cout << "=====================================================================================\n\n";
        opsiLandingPage();
        cout << "\nPilih Opsi : ";
        cin >> opt;
        stat_exit = 1;

        while (opt != 99)
        {
            switch (opt)
            {
            case 1:system("cls"); halTarikTunai(); break;
            case 2:system("cls"); halSetorTunai(); break;
            case 3:system("cls"); halTransfer(); break;
            case 4:system("cls"); halCekSaldo(); break;
            case 5:system("cls"); halDaftarMutasi(); break;
            case 6:system("cls"); halPelayanan(); break;
            default:
                cout << "Masukkan input yang valid!\n\n";
                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore();
                }
                break;
            }
            if (stat_exit == 1)
            {
                system("pause");
            }
            system("cls");
            cout << "Pilih Transaksi Pembayaran\n";
            cout << "=====================================================================================\n\n";
            opsiLandingPage();
            cout << "\nPilih Opsi : ";
            cin >> opt;
            stat_exit = 1;
        }
        exit(1);
}

void opsiLandingPage() {
    cout << "1. Tarik Tunai\n";
    cout << "2. Setor Tunai\n";
    cout << "3. Transfer\n";
    cout << "4. Info Saldo\n";
    cout << "5. Daftar Mutasi\n";
    cout << "6. Opsi Pelayanan\n\n";
    cout << "99. Keluar\n";
}

void halCekSaldo() {
    validasiStatusKartu(email);
    if (statuskartu == "1")
    {
        system("cls");
        halUtama();
    }

    system("cls");
    cout << "Jumlah Saldo yang Anda Miliki\n";
    cout << "=====================================================================================\n";
    getSaldofromDB(email);
}

void halTarikTunai() {
    validasiStatusKartu(email);
    if (statuskartu == "1")
    {
        system("cls");
        halUtama();
    }

    int opt;
    int nominal;

input:
    cout << "Tarik Tunai\n";
    cout << "=====================================================================================\n";
    cout << "1. Rp. 50.000\n";
    cout << "2. Rp. 100.000\n";
    cout << "3. Rp. 200.000\n";
    cout << "4. Rp. 300.000\n";
    cout << "5. Rp. 500.000\n";
    cout << "6. Rp. 1.000.000\n";
    cout << "7. Isi Nominal\n";
    cout << "8. Kembali\n\n";
    cout << "Masukkan Opsi : ";
    cin >> opt;

    while (opt != 8)
    {
        switch (opt)
        {
        case 1:nominal = 50000; systemTarikTunai(nominal, email); break;
        case 2:nominal = 100000; systemTarikTunai(nominal, email); break;
        case 3:nominal = 200000; systemTarikTunai(nominal, email); break;
        case 4:nominal = 300000; systemTarikTunai(nominal, email); break;
        case 5:nominal = 500000; systemTarikTunai(nominal, email); break;
        case 6:nominal = 1000000; systemTarikTunai(nominal, email); break;
        case 7:
            cout << "Masukkan Nominal yang diinginkan : ";
            cin >> nominal; 
            if (nominal % 50000 != 0 || nominal % 100000 != 0)
            {
                cout << "\nTarik Tunai hanya menerima kelipatan Rp. 50,000 dan Rp. 100,000\n\n";
                system("pause");
                system("cls");
                goto input;
            }
            systemTarikTunai(nominal, email);
            break;
        default:
            cout << "Masukkan Opsi yang Valid";
            system("pause");
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }
            break;
        }
        if (stat_exit==0)
        {
            system("cls");
            halUtama();
        }
        system("cls");
        cout << "Tarik Tunai\n";
        cout << "=====================================================================================\n";
        cout << "1. Rp. 50.000\n";
        cout << "2. Rp. 100.000\n";
        cout << "3. Rp. 200.000\n";
        cout << "4. Rp. 300.000\n";
        cout << "5. Rp. 500.000\n";
        cout << "6. Rp. 1.000.000\n";
        cout << "7. Isi Nominal\n";
        cout << "8. Kembali\n\n";
        cout << "Masukkan Opsi : ";
        cin >> opt;
    }

    system("cls"); halUtama();
}

void halSetorTunai() {
    validasiStatusKartu(email);
    if (statuskartu == "1")
    {
        system("cls");
        halUtama();
    }

    int nominal_setor=0;
    while (nominal_setor!=01)
    {
        cout << "Setor Tunai\n";
        cout << "=====================================================================================\n\n";
        cout << "00.Kembali\n\n";
        cout << "01.Cancel\n\n";
        cout << "Masukkan Jumlah Uang yang Akan disetorkan : Rp. ";
        cin >> nominal_setor;
        

        if (cin.fail())
        {
            system("cls");
            cout << "Setor Tunai\n";
            cout << "=====================================================================================\n\n";
            cout << "Masukkan input yang valid!\n";
            system("pause");
            cin.clear();
            cin.ignore();
            system("cls");
        }
        else if(nominal_setor == 00){
            system("cls");
            halUtama();
        }
        else if ( nominal_setor % 50000 ==  0 || nominal_setor % 100000 == 0) {
            systemSetorTunai(nominal_setor, email);
            if (stat_exit == 0)
            {
                system("cls");
                halUtama();
            }
        }
        else {
            system("cls");
            cout << "Setor Tunai\n";
            cout << "=====================================================================================\n\n";
            cout << "Mohon Maaf, Kelipatan Setoran yang diterima adalah Rp. 50.000 dan Rp. 100.000\n\n";
            system("pause");
            system("cls");
        }
    }
    cout << "Setor Tunai\n";
    cout << "=====================================================================================\n\n";
    exit(1);
}

void halTransfer() {
    validasiStatusKartu(email);
    if (statuskartu == "1")
    {
        system("cls");
        halUtama();
    }

    int opsi;

    cout << "Transfer\n";
    cout << "=====================================================================================\n\n";
    cout << "1.Antar Rekening\n";
    cout << "2.Antar Bank\n";
    cout << "3.Virtual Account\n";
    cout << "4.Kembali\n\n";
    cout << "99.Cancel\n\n";

    cout << "Masukkan Opsi : ";
    cin >> opsi;

    while (opsi != 99)
    {
        switch (opsi)
        {
        case 1: system("cls");  antarRekening(); break;
        case 2: system("cls");  antarBank(); break;
        case 3: system("cls");  virtualAcc(); break;
        case 98: system("cls"); halUtama(); break;
        default:
            cout << "Masukkan Input yang Valid!";
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }
            break;
        }
        system("pause");
        system("cls");
        cout << "Transfer\n";
        cout << "=====================================================================================\n\n";
        cout << "1.Antar Rekening\n";
        cout << "2.Antar Bank\n";
        cout << "3.Virtual Account\n\n";
        cout << "98.Kembali\n";
        cout << "99.Cancel\n\n";

        cout << "Masukkan Opsi : \n";
        cin >> opsi;
    }
    exit(1);
}

void antarRekening() {
input:
    string no_rek;
    cout << "Transfer Antar Rekening\n";
    cout << "=====================================================================================\n";
    cout << "Kode Rekening Bank Nur Muttaqin : 221524XXX\n\n";

    cout << "Masukkan Nomor Rekening : ";
    cin >> no_rek;

    while (strlen(no_rek.c_str()) != 9)
    {
        cout << "\nNomor Rekening tidak sesuai! Tolong cek kembali nomor yang anda masukkan.\n\n";
        system("pause");
        system("cls");
        goto input;
    }

    validasiNoRekAntarRek(email,no_rek);
    if (stat_exit == 0)
    {
        system("cls");
        halUtama();
    }
}

void antarBank() {

}

void virtualAcc() {

}

void halDaftarMutasi() {
    validasiStatusKartu(email);
    if (statuskartu == "1")
    {
        system("cls");
        halUtama();
    }

input:
    int jml;
    cout << "Daftar Mutasi\n";
    cout << "=====================================================================================\n\n";
    cout << "Masukkan Jumlah Riwayat Transaksi Terakhir yang Anda Ingin Lihat : ";
    cin >> jml;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore();
        system("cls");
        goto input;
    }

    cout << "\n";
    cout << "Riwayat Transaksi\n\n";
    getDaftarMutasi(email, jml);
}

void halPelayanan() {
    int opsi;

    cout << "Opsi Pelayanan\n";
    cout << "=====================================================================================\n\n";
    cout << "1.Ubah PIN\n";
    cout << "2.Ubah E-Mail\n";
    cout << "3.Ubah Status Aktivasi Kartu\n\n";

    cout << "Masukkan Opsi : ";
    cin >> opsi;

    while (opsi!=99)
    {
        switch (opsi)
        {
        case 1:system("cls");ubahPIN(); break;
        case 2:system("cls");ubahEmail(); break;
        case 3:system("cls");ubahAktivasi(); break;
        default:
            cout << "Masukkan Input yang Valid!";
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }
            break;
        }
    }
    system("cls"); halUtama();
}

void ubahPIN() {
input:
    string tgl_lahir;
    int pinbaru;
    int konfirmasipin;

    if (tgl_lahir_statubahpin == 0)
    {
        cout << "Ubah PIN\n";
        cout << "=====================================================================================\n\n";
        cout << "99.Kembali\n\n";

        cout << "Konfirmasi Tanggal Lahir Anda (DD/MM/YYYY) : ";
        cin >> tgl_lahir;
        if (tgl_lahir == "99")
        {
            system("cls");
            halUtama();
        }
        validasiTglLahir(tgl_lahir, email);
    }
    else {
    input_pin:
        system("cls");
        cout << "Ubah PIN\n";
        cout << "=====================================================================================\n\n";
        cout << "99.Batal\n\n";

        cout << "Masukkan PIN Baru : ";
        cin >> pinbaru;

        if (pinbaru == 99)
        {
            system("cls");
            halUtama();
        }

        while (cin.fail())
        {
            cout << "PIN Harus Berupa Angka!\n";
            system("pause");
            cin.clear();
            cin.ignore();
            goto input_pin;
        }

        stringstream inttostr;
        inttostr << pinbaru;
        string pinstr;
        inttostr >> pinstr;

        while (strlen(pinstr.c_str()) != 6)
        {
            cout << "PIN Wajib 6-digit!\n";
            system("pause");
            system("cls");
            goto input_pin;
        }

        cout << "\n";
    input_konfirmasi:
        cout << "Masukkan Konfirmasi PIN Baru : ";
        cin >> konfirmasipin;

        while (cin.fail())
        {
            cout << "PIN Harus Berupa Angka!\n";
            system("pause");
            cin.clear();
            cin.ignore();
            goto input_konfirmasi;
        }
        
        while (konfirmasipin != pinbaru) {
            cout << "Konfirmasi PIN salah!\n";
            system("pause");
            cout << "\n";
            goto input_konfirmasi;
        }

        systemUbahPIN(email, konfirmasipin);
        system("cls");
        halUtama();
    }
    system("cls");
    goto input;
}

void ubahEmail() {
input:
    string tgl_lahir;
    string emailbaru;
    string konfirmasiemail;

    if (tgl_lahir_statubahemail == 0)
    {
        cout << "Ubah E-Mail\n";
        cout << "=====================================================================================\n\n";
        cout << "99.Kembali\n\n";

        cout << "Konfirmasi Tanggal Lahir Anda (DD/MM/YYYY) : ";
        cin >> tgl_lahir;
        if (tgl_lahir == "99")
        {
            system("cls");
            halUtama();
        }
        validasiTglLahir(tgl_lahir, email);
    }else {
    input_email:
        system("cls");
        cout << "Ubah E-Mail\n";
        cout << "=====================================================================================\n\n";
        cout << "99.Batal\n\n";

        cout << "Masukkan Email Baru : ";
        cin >> emailbaru;

        if (emailbaru == "99")
        {
            system("cls");
            halUtama();
        }

        cout << "\n";

    input_konfirmasi:
        cout << "Masukkan Konfirmasi E-Mail Baru : ";
        cin >> konfirmasiemail;

        while (konfirmasiemail != emailbaru) {
            cout << "Konfirmasi E-Mail salah!\n";
            system("pause");
            cout << "\n";
            goto input_konfirmasi;
        }

        systemUbahEmail(email, konfirmasiemail);
        system("cls");
        email = konfirmasiemail;
        halUtama();
    }
    system("cls");
    goto input;
}

void ubahAktivasi() {
input:
    string tgl_lahir;
    int opsi;
    int value;

    if (tgl_lahir_statusubahaktivasi == 0)
    {
        cout << "Ubah Status Aktivasi\n";
        cout << "=====================================================================================\n\n";
        cout << "99.Kembali\n\n";

        cout << "Konfirmasi Tanggal Lahir Anda (DD/MM/YYYY) : ";
        cin >> tgl_lahir;
        if (tgl_lahir == "99")
        {
            system("cls");
            halUtama();
        }
        validasiTglLahir(tgl_lahir, email);
    }
    else {
    inputaktivasi:
        system("cls");
        cout << "Ubah Status aktivasi\n";
        cout << "=====================================================================================\n\n";
        cout << "1.Aktifkan Akun\n";
        cout << "2.Non-Aktifkan Akun\n\n";
        cout << "99.Batal\n\n";

        cout << "Masukkan Opsi : ";
        cin >> opsi;

        if (opsi == 99)
        {
            system("cls");
            halUtama();
        }

        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "Masukkan Input yang Valid";
            system("pause");
            goto inputaktivasi;
        }

        while ( opsi != 1 && opsi != 2)
        {
            cout << "Masukkan Input yang Valid";
            system("pause");
            goto inputaktivasi;
        }

        if (opsi == 1)
        {
            value = 0;
        }
        else if (opsi == 2) {
            value = 1;
        }

        systemStatAktivasi(email, value);
        system("cls");
        statuskartu = value;
        halUtama();
    }
    system("cls");
    goto input;
}
