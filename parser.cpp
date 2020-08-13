//
// Created by Jatin Prakash on 07-08-2020.

#include <bits/stdc++.h>
using namespace std;
#define forn(i, n)for(int i=0;i<int(n);i++)
#define sz(a) int(a.size())
template <class A, class B> ostream& operator << (ostream& out, const pair<A, B> &a){return out << "(" << a.first << ", " << a.second << ")";}
template<class A>ostream& operator<<(ostream& out,const vector<A>&v){out<<"[";forn(i,sz(v)){if(i)out<<", ";out<<v[i];}return out<<"]";}
#define watch(x) cout << #x << " : " << x << endl;

vector<vector<string> > mips_code;

// trim the input string...
string trim(string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Match registers with number assigned to them
string binary_reg(const string& reg){
    string answer;
    if(reg == "$zero")      answer = "00000";
    else if(reg == "$v0")   answer = "00010";
    else if(reg == "$v1")   answer = "00011";
    else if(reg == "$a0")   answer = "00100";
    else if(reg == "$a1")   answer = "00101";
    else if(reg == "$a2")   answer = "00110";
    else if(reg == "$a3")   answer = "00111";
    else if(reg == "$t0")   answer = "01000";
    else if(reg == "$t1")   answer = "01001"; // 9
    else if(reg == "$t2")   answer = "01010"; // 10
    else if(reg == "$t3")   answer = "01011";
    else if(reg == "$t4")   answer = "01100";
    else if(reg == "$t5")   answer = "01101";
    else if(reg == "$t6")   answer = "01110";
    else if(reg == "$t7")   answer = "01111";
    else if(reg == "$s0")   answer = "10000";
    else if(reg == "$s1")   answer = "10001";
    else if(reg == "$s2")   answer = "10010";
    else if(reg == "$s3")   answer = "10011";
    else if(reg == "$s4")   answer = "10100";
    else if(reg == "$s5")   answer = "10101";
    else if(reg == "$s6")   answer = "10110";
    else if(reg == "$s7")   answer = "10111";
    else if(reg == "$t8")   answer = "11000";
    else if(reg == "$t9")   answer = "11001";
    else if(reg == "$gp")   answer = "11100";
    else if(reg == "$sp")   answer = "11101";
    else if(reg == "$fp")   answer = "11110";
    else if(reg == "$ra")   answer = "11111";
    else answer = " Invalid Register ";

    return answer;
}

string intToBinary(const string& num, int size)
{
    char binaryNum[32];
    for(int i=0; i<32; i++) binaryNum[i] = '0';

    // counter for binary array
    int i = 0;
    int n = stoi(num);
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[31-i] = n % 2 + 48;
        n = n / 2;
        i++;
    }

    string answer = "";
    for(int i=32 - size; i<32; i++)   answer = answer +  binaryNum[i];
    return answer;

}

void parse_bracket(string &line, string &reg, string &offset){
    int size = line.size();
    int j = 0;
    for(j = 0; j<size; j++){
        if(line[j] == '(') break;
        offset += line[j];
    }
    for(int i=j+1; i<size; i++){
        if(line[i] == ')') break;
        reg += line[i];
    }
}

int find_string(string line){
    int size = mips_code.size();
    for(int i=0; i<size; i++){
        if(trim(mips_code[i][0]) == trim(line) ) return i;   //if found..
    }
    return -1;  //if not found..
}

void mipsCode2MachineCode(vector <string> &instruc, string &binary_instruc,int pc){
    int size = instruc.size();
    //int num_labels = numberOfLabels();
    if(size == 0){
        binary_instruc = "00000000000000000000000000000000\n";
        return ;
    }

    if(instruc[0] == "add"){

        binary_instruc += "000000" + binary_reg(instruc[2]);
        binary_instruc += binary_reg(instruc[3]) + binary_reg(instruc[1]) + "00000" + "100000" + "\n";
    }
    else if(instruc[0] == "sub"){

        binary_instruc = "000000" + binary_reg(instruc[2]);
        binary_instruc += binary_reg(instruc[3]) + binary_reg(instruc[1]) + "00000" + "100010" + "\n";
    }
    else if(instruc[0] == "sll"){

        binary_instruc = "00000000000";
        binary_instruc += binary_reg(instruc[2]) + binary_reg(instruc[1]) + intToBinary(instruc[3], 5) + "000000" + "\n";
    }
    else if(instruc[0] == "srl"){

        binary_instruc = "00000000000";
        binary_instruc += binary_reg(instruc[2]) + binary_reg(instruc[1]) + intToBinary(instruc[3], 5) + "000010" + "\n";
    }
    else if(instruc[0] == "lw"){

        binary_instruc = "100011" ;
        string reg; string offset;
        parse_bracket(instruc[2], reg, offset);
        binary_instruc += binary_reg(reg) + binary_reg(instruc[1]) + intToBinary(offset, 16) + "\n";
    }
    else if(instruc[0] == "sw"){

        binary_instruc = "101011" ;
        string reg; string offset;
        parse_bracket(instruc[2], reg, offset);
        binary_instruc += binary_reg(reg) + binary_reg(instruc[1]) + intToBinary(offset, 16) + "\n";
    }
    else if(instruc[0] == "bne"){

        binary_instruc += "000101" + binary_reg(instruc[1]) + binary_reg(instruc[2]);
        string label;
        label += instruc[3] + ":";
        int pos = find_string(label);
        label = to_string(pos+1);
        binary_instruc += intToBinary(label, 16) + "\n";
    }
    else if(instruc[0] == "beq"){

        binary_instruc += "000100" + binary_reg(instruc[1]) + binary_reg(instruc[2]);
        string label;
        label += instruc[3] + ":";
        int pos = find_string(label);
        label = to_string(pos+1 - (pc+1) );
        binary_instruc += intToBinary(label, 16) + "\n";
    }
    else if(instruc[0] == "bgtz"){

        binary_instruc += "000111" + binary_reg(instruc[1]) + "00000";
        string label;
        label += instruc[2] + ":";
        int pos = find_string(label);
        label = to_string(pos+1);
        binary_instruc += intToBinary(label, 16) + "\n";
    }
    else if(instruc[0] == "blez"){

        binary_instruc += "000110" + binary_reg(instruc[1]) + "00000";
        string label;
        label += instruc[2] + ":";
        int pos = find_string(label);
        label = to_string(pos+1);
        binary_instruc += intToBinary(label, 16) + "\n";
    }
    else if(instruc[0] == "jal"){

        binary_instruc += "000011";
        string label;
        label += instruc[1] + ":";
        int pos = find_string(label);
        label = to_string(pos+1);
        binary_instruc += intToBinary(label, 26) + "\n";
    }
    else if(instruc[0] == "jr"){
        binary_instruc += "00000011111000000000000000001000\n";
    }
    else if(instruc[0] == "j"){

        binary_instruc += "000010";
        string label;
        label += instruc[1] + ":";
        int pos = find_string(label);
        label = to_string(pos+1);
        binary_instruc += intToBinary(label, 26) + "\n";
    }
    else if(instruc[0] == "exit")
    {
       binary_instruc += "10101010101010101010101010101010\n";
    }
    else{
        binary_instruc += "11111111111111111111111111111111\n";
    }
}

void convert_to_machine_code()
{
    string line;
    string word;

    while(getline(cin, line)){
        vector <string> instruc;
        istringstream check(line);
        while(check >> word)    instruc.push_back(word);
        mips_code.push_back(instruc);
    }

    int count = 0;  //actual number of executable instructions..

    int size = mips_code.size();    //size is the number of instructions along with lables
    string binary_instruc;
    for(int i=0; i<size; i++){
        mipsCode2MachineCode(mips_code[i], binary_instruc,i);
        if(!binary_instruc.empty())  count++;
        cout << binary_instruc;
        binary_instruc = "";
    }
}

int main(){
    convert_to_machine_code();
    return 0;
}
