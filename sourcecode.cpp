#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

using namespace std;

string wire(int b, int a) {
    if (b == a) {
        return "";
    }
    string str = "";
    int mid = (b + a + 1) / 2;
    int mid1 = mid - 1;
    for (int i = mid; i <= b; i++) {
        if (a == 0) {
            str += "\twire G" + to_string(i) + "_" + to_string(a) + ";\n";
        }
        else {
            str += "\twire G" + to_string(i) + "_" + to_string(a) + ", P" + to_string(i) + "_" + to_string(a) + ";\n";
        }
    }
    string str1 = wire(mid1,a);
    string str2 = wire(b,mid);
    return str1+str2+str;
}

string init_sk_adder(int b) {
    string str = "module sklansky(ci, a, b, s, co);\n"
        "\tinput ci; \n\tinput[" + to_string(b) + ":1] a, b;"
        " \n\toutput co; \n\toutput[" + to_string(b) + ":1] s;\n\n";
    for (int j = 0; j <= b; j++) {
        str += "\twire G" + to_string(j) + "_" + to_string(j) + ", P" + to_string(j) + "_" + to_string(j) + ";\n";
    }
    str += "\n" + wire(b - 1, 0);
    str += "\n\tassign G0_0 = ci;\n\tassign P0_0 = 0;\n\n";
    for (int i = 1; i <= b; i++) {
        str += "\tinitGP A" + to_string(i) + "(a[" + to_string(i) + "], b[" + to_string(i) + "], G" + to_string(i) + "_" + to_string(i) + ", P" + to_string(i) + "_" + to_string(i) + ");\n";
    }
    return str+"\n";
}

string g_bcells(int b, int a) {
    string str = "";
    int mid = (b + a + 1) / 2;
    int mid1 = mid - 1;
    if (mid == b) {
        if (mid1 == 0) {
            str = "\tG_Cell gcell1_0(G1_1,P1_1,G0_0,G1_0);\n";
        }
        else {
            str = "\tB_Cell bcell" + to_string(mid) + "_" + to_string(mid1) + "(G" + to_string(mid) +
                "_" + to_string(mid) + ",P" + to_string(mid) + "_" + to_string(mid) + ",G" +
                to_string(mid1) + "_" + to_string(mid1) + ",P" + to_string(mid1) + "_" + to_string(mid1) +
                ",G" + to_string(mid) + "_" + to_string(mid1) + ",P" + to_string(mid) + "_" + to_string(mid1) + ");\n";
        }
        return str;
    }
    else {
        for (int i = mid; i <= b; i++) {
            if (a == 0) {
                str += "\tG_Cell gcell" + to_string(i) + "_" + to_string(a) + "(G" + to_string(i) +
                    "_" + to_string(mid) + ",P" + to_string(i) + "_" + to_string(mid) + ",G" + to_string(mid1) +
                    "_" + to_string(a) + ",G" + to_string(i) + "_" + to_string(a) + ");\n";
            }
            else {
                str += "\tB_Cell bcell" + to_string(i) + "_" + to_string(a) + "(G" + to_string(i) +
                    "_" + to_string(mid) + ",P" + to_string(i) + "_" + to_string(mid) +
                    ",G" + to_string(mid1) + "_" + to_string(a) + ",P" + to_string(mid1) + "_" + to_string(a) +
                    ",G" + to_string(i) + "_" + to_string(a) + ",P" + to_string(i) + "_" + to_string(a) + ");\n";
            }
        }

        string str1 = g_bcells(mid1, a);
        string str2 = g_bcells(b, mid);

        return str1 + str2 + str;
    }
}

string end_sk_adder(int b) {
    string str = "";
    for (int i = 1; i <= b; i++) {
        str += "\tassign s[" + to_string(i) + "] = P" + to_string(i) + "_" + to_string(i) +
            " ^ G" + to_string(i - 1) + "_" + to_string(0)+";\n";
    }
    str += "\n\twire w;\n\tassign w = P" + to_string(b) + "_" + to_string(b) + " & G" + to_string(b - 1) + "_" +
        to_string(0) + ";\n\tassign co = w|G" + to_string(b) + "_" + to_string(b)+";\nendmodule\n";
    return str;
}

void generate_sk_adder(int n){
    string str = "module initGP(a, b, g, p);\n\tinput a, b;\n\toutput g, p;\n\tassign g = a & b;\n\tassign p = a ^ b;\nendmodule\n\n"
        "module B_Cell(Gi1, Pi1, Gi2, Pi2, Go, Po);\n\tinput Gi1, Gi2, Pi1, Pi2;\n\toutput Go, Po;\n\twire w;"
        "\tassign w = Pi1 & Gi2;\n\tassign Go = Gi1 | w;\n\tassign Po = Pi1 & Pi2;\nendmodule\n\n"
        "module G_Cell(Gi1, Pi1, Gi2, Go);\n\tinput Gi1, Gi2, Pi1;\n\toutput Go;\n\twire w;\n"
        "\tassign w = Pi1 & Gi2;\n\tassign Go = Gi1 | w;\nendmodule\n\n";
    str += init_sk_adder(n);
    str += g_bcells(n - 1, 0) + "\n";
    str += end_sk_adder(n);
    ofstream fw(to_string(n)+"bit_sk_adder.v", std::ofstream::out);
    if (fw.is_open())
    {
        fw << str;
        fw.close();
    }
    else cout << "Problem with opening file";
}

int check_n(int n) {
    if (n > 0) {
        while (n % 2 == 0) {
            n /= 2;
        }
        if (n == 1) {
            return 1;
        }
    }
    else {
        return 0;
    }
    if (n == 0 || n != 1) {
        return 0;
    }
    return 0;
}

string rand_nbit_bin(int n) {
    static int m = 0;
    m++;
    srand(m);
    string str = "";
    for (int i = 0; i < n; i++) {
        str += to_string(rand() % 2);
    }
    return str;
}

void gennerate_test_bench(int n) {
    string n_str = to_string(n);
    string n1_str = to_string(n - 1);

    string str = "module skadd_testb();\n\treg[" + n1_str + ":0] a, b;\n\treg ci;\n\twire[" + n1_str + ":0] s;"
        "\twire co;\n\tsklansky g1(ci, a[" + n1_str + ":0], b[" + n1_str + ":0], s[" + n1_str +":0], co);\n\tinitial begin\n";
    str += "\t\ta = " + n_str + "'b"+rand_nbit_bin(n)+"; b=" + n_str + "'b" + rand_nbit_bin(n) + "; ci = 1'b0;\n";
    str += "\t\t#5 a = " + n_str + "'b" + rand_nbit_bin(n) + "; b=" + n_str + "'b" + rand_nbit_bin(n) + "; ci = 1'b0;\n";
    str += "\t\t#5 a = " + n_str + "'b" + rand_nbit_bin(n) + "; b=" + n_str + "'b" + rand_nbit_bin(n) + "; ci = 1'b1;\n";
    str += "\t\t#5 a = " + n_str + "'b" + rand_nbit_bin(n) + "; b=" + n_str + "'b" + rand_nbit_bin(n) + "; ci = 1'b1;\n";
    str += "\tend\nendmodule";
    ofstream fw(to_string(n) + "bit_test_becnh.v", std::ofstream::out);
    if (fw.is_open())
    {
        fw << str;
        fw.close();
    }
    else cout << "Problem with opening file";
}

int main()
{
    int n;
    cout << "Enter N:";
    cin >> n;
    if (check_n(n)) {
        generate_sk_adder(n);
        cout << "Verilog code has been created successfully.";
        cout << "\nDo you require a test bench (y/n):";
        char response;
        cin >> response;
        if (response == 'y') {
            gennerate_test_bench(n);
            cout<<"Test bench has been created successfully";
        }
    }
    else {
        cout << "Invalid input";
    }
    return 0;
   
}
